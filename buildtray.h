#ifndef BUILDTRAY_H
#define BUILDTRAY_H

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui {
class BuildTray;
}

class BuildTray : public QMainWindow
{
    Q_OBJECT

public:
    explicit BuildTray(QWidget *parent = 0);
    ~BuildTray();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionAbout_triggered();
    void trayInit();
    void trayActivated();

private:
    Ui::BuildTray *ui;
    QSystemTrayIcon *tray;
};

#endif // BUILDTRAY_H
