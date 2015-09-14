#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QtSql>
#include <QProcess>

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();
    int cron;
    int activeJob;
    int lastJob;
    QProcess job;
signals:
    void notify(QString title, QString text);
    void log(QString text);
    void tableEnable(bool status);

public slots:
    bool initDB(QSqlDatabase database);

private slots:
    void watchdog();
    void setupJob(int newJob);
    void readJob();
    void endJob(int status);
    void errorJob(QProcess::ProcessError);

private:
    QSqlDatabase db;
};

#endif // MANAGER_H
