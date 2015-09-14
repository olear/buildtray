#ifndef BUILDTRAY_H
#define BUILDTRAY_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "manager.h"
#include <QtSql>

namespace Ui {
class BuildTray;
}

class BuildTray : public QMainWindow
{
    Q_OBJECT

public:
    explicit BuildTray(QWidget *parent = 0);
    ~BuildTray();
    QSqlDatabase db;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionAbout_triggered();
    void trayInit();
    void trayActivated();
    void managerInit();
    void settingsLoad();
    void settingsSave();

public slots:
    void notify(QString title, QString text);
    void log(QString text);

private:
    Ui::BuildTray *ui;
    QSystemTrayIcon *tray;
    Manager *manager;
    QSqlTableModel *jobsModel;
};

#endif // BUILDTRAY_H
