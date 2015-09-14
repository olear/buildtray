#include "buildtray.h"
#include "ui_buildtray.h"
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QSettings>

BuildTray::BuildTray(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BuildTray)
{
    ui->setupUi(this);
    tray = new QSystemTrayIcon(this);
    manager = new Manager;
    QTimer::singleShot(0,this,SLOT(trayInit()));
    QTimer::singleShot(0,this,SLOT(managerInit()));
}

BuildTray::~BuildTray()
{
    settingsSave();
    delete ui;
}

void BuildTray::on_actionQuit_triggered()
{
    if (manager->job.isOpen())
        QMessageBox::warning(this,tr("Job(s) active"),tr("Job(s) still running, unable to quit"));
    else
        qApp->quit();
}

void BuildTray::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,tr("About Qt"));
}

void BuildTray::on_actionAbout_triggered()
{
    QMessageBox::about(this,tr("About BuildTray"),tr("Manage build scripts<br><br>Written by <a href=\"https://github.com/olear\">Ole-Andr&eacute; Rodlie</a>"));
}


void BuildTray::closeEvent(QCloseEvent *event)
{
    if (tray->isVisible()) {
        this->hide();
        tray->showMessage(tr("Still running"),tr("Application still running in systray"));
        event->ignore();
    }
    else {
        if (manager->job.isOpen()) {
            QMessageBox::warning(this,tr("Job(s) active"),tr("Job(s) still running, unable to quit"));
            event->ignore();
        }
        else
            event->accept();
    }
}

void BuildTray::trayInit()
{
    // settings
    settingsLoad();

    // database
    db=QSqlDatabase::addDatabase("QSQLITE");
    if (!db.isValid()) {
        QMessageBox::warning(this,tr("Database error"),tr("Unable to use default SQL driver, please check installation"));
        QTimer::singleShot(1000,qApp,SLOT(quit()));
    }
    db.setDatabaseName(QDir::homePath()+QDir::separator()+".buildtray.sqlite");
    bool dbStatus = false;
    if (db.open()) {
        QSqlQuery query;
        QString autoinc;
        if (db.driverName()=="QMYSQL")
            autoinc="AUTO_INCREMENT";
        if (db.driverName()=="QSQLITE")
            autoinc="AUTOINCREMENT";
        if (query.exec("CREATE TABLE IF NOT EXISTS jobs ("
                       "id INTEGER PRIMARY KEY "+autoinc+","
                       "desc VARCHAR(255) UNIQUE NOT NULL,"
                       "dir VARCHAR(255) NOT NULL,"
                       "timeout INTEGER(10) NULL,"
                       "script VARCHAR(255) NOT NULL"
                       ");"))
            dbStatus=true;
        jobsModel = new QSqlTableModel(0,db);
        jobsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
        jobsModel->setTable("jobs");
        jobsModel->select();
        if (jobsModel->rowCount()<1)
            jobsModel->insertRow(jobsModel->rowCount(QModelIndex()));
        ui->jobs->setModel(jobsModel);
        ui->jobs->hideColumn(0);
        ui->jobs->setContextMenuPolicy(Qt::NoContextMenu);
        ui->jobs->horizontalHeader()->setStretchLastSection(true);
    }
    if (!dbStatus) {
        QMessageBox::warning(this,tr("Database error"),tr("Failed to communicate with database, please check installation"));
        QTimer::singleShot(1000,qApp,SLOT(quit()));
    }

    // tray
    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayActivated()));
    tray->setIcon(QIcon(":/res/icons/buildtray-22px.png"));
    tray->setToolTip("BuildTray");
    if (tray->isSystemTrayAvailable())
        tray->show();
    if (tray->isVisible())
        this->hide();
}

void BuildTray::trayActivated()
{
    if (this->isHidden())
        this->show();
    else
        this->hide();
}

void BuildTray::managerInit()
{
    connect(manager,SIGNAL(notify(QString,QString)),this,SLOT(notify(QString,QString)));
    connect(manager,SIGNAL(log(QString)),this,SLOT(log(QString)));
    connect(manager,SIGNAL(tableEnable(bool)),ui->jobs,SLOT(setEnabled(bool)));
    if (!manager->initDB(db)) {
        QMessageBox::warning(this,tr("Database error"),tr("Failed to communicate with database, please check installation"));
        QTimer::singleShot(1000,qApp,SLOT(quit()));
    }
}

void BuildTray::notify(QString title, QString text)
{
    if (!title.isEmpty() && !text.isEmpty()) {
        if (tray->isVisible())
            tray->showMessage(title,text);
        else {
            qDebug() << title << ":" << text;
        }
    }
}

void BuildTray::log(QString text)
{
    if (!text.isEmpty())
        ui->terminal->appendPlainText(text);
}

void BuildTray::settingsLoad()
{
    QSettings settings;
    settings.beginGroup("ui");
    if (settings.value("state").isValid())
        this->restoreState(settings.value("state").toByteArray());
    if (settings.value("size").isValid())
        this->resize(settings.value("size",QSize(640,480)).toSize());
    if (settings.value("pos").isValid())
        this->move(settings.value("pos",QPoint(0,0)).toPoint());
    if (settings.value("max").toBool()==true)
        this->showMaximized();
    settings.endGroup();
}

void BuildTray::settingsSave()
{
    if (db.isOpen())
        db.close();
    QSettings settings;
    settings.beginGroup("ui");
    settings.setValue( "state",this->saveState());
    settings.setValue("size",this->size());
    settings.setValue("pos",this->pos());
    if (this->isMaximized())
        settings.setValue("max","true");
    else
        settings.setValue("max","false");
    settings.endGroup();
    settings.sync();
}
