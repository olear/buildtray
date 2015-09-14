#include "manager.h"
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QDebug>

Manager::Manager(QObject *parent) :
    QObject(parent)
{
    cron = 300000; // override in managerInit
    activeJob = 0;
    lastJob = 0;
    connect(&job,SIGNAL(readyRead()),this,SLOT(readJob()));
    connect(&job,SIGNAL(finished(int)),this,SLOT(endJob(int)));
    connect(&job,SIGNAL(error(QProcess::ProcessError)),this,SLOT(errorJob(QProcess::ProcessError)));
    QTimer::singleShot(10000,this,SLOT(watchdog()));
}

Manager::~Manager()
{
    if (job.isOpen())
        job.close();
    if (db.isOpen())
        db.close();
}

void Manager::watchdog()
{
    // disable ui
    emit tableEnable(false);

    // get jobs
    if (db.isOpen() && !job.isOpen()) {
        QSqlQuery query;
        int jobs = 0;
        if (query.exec("SELECT id from jobs;")) {
            while(query.next())
                jobs++;
        }
        if (jobs>0) {
            emit notify(tr("Jobs"),tr("added ")+QString::number(jobs)+tr(" jobs to queue"));
            if (jobs>=lastJob+1)
                setupJob(lastJob+1);
            else {
                lastJob=0;
                setupJob(lastJob+1);
            }
        }
    }
    else {
        if (!db.isOpen())
            emit notify(tr("Database error"),tr("Unable to open database"));
        if (job.isOpen())
            emit notify(tr("Job(s) active"),tr("Job(s) still running"));
    }

    // enable ui
    emit tableEnable(true);

    // loop
    QTimer::singleShot(cron,this,SLOT(watchdog()));
}

bool Manager::initDB(QSqlDatabase database)
{
    bool status = false;
    if (database.isValid()) {
        db=database;
        if (db.open())
            status=true;
    }
    return status;
}

void Manager::setupJob(int newJob)
{
    if (newJob>0) {
        if (job.isOpen())
            job.close();
        emit notify(tr("New job added"),tr("processing new job..."));
        job.setProcessChannelMode(QProcess::MergedChannels);
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QFile bash("/bin/bash");
        if (bash.exists())
            env.insert("SH","/bin/bash");
        QString workdir;
        QString exec;
        QSqlQuery query;
        if (query.exec("SELECT dir,script FROM jobs WHERE id="+QString::number(newJob))) {
            while(query.next()) {
                workdir=query.value(0).toString();
                exec=query.value(1).toString();
            }
        }
        if (!workdir.isEmpty() && !exec.isEmpty()) {
            activeJob=newJob;
            lastJob=newJob;
            emit log(tr("Starting job ")+QString::number(activeJob)+tr(" at ")+QDateTime::currentDateTime().toString());
            job.setWorkingDirectory(workdir);
            job.setProcessEnvironment(env);
            job.start(workdir+QDir::separator()+exec);
        }
    }
}

void Manager::readJob()
{
    emit log(job.readAll());
}

void Manager::endJob(int status)
{
    emit log(tr("Job ended at ")+QDateTime::currentDateTime().toString());
    job.close();
    activeJob=0;
    if (status==0) {
        notify(tr("Job done"),tr("Job done"));
    }
    else {
        notify(tr("Job error"),tr("Job failed, see log"));
    }
}

void Manager::errorJob(QProcess::ProcessError)
{
    emit log(job.errorString());
    endJob(1);
}
