#include "buildtray.h"
#include "ui_buildtray.h"
#include <QMessageBox>
#include <QTimer>

BuildTray::BuildTray(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BuildTray)
{
    ui->setupUi(this);
    tray = new QSystemTrayIcon(this);
    QTimer::singleShot(0,this,SLOT(trayInit()));
}

BuildTray::~BuildTray()
{
    delete ui;
}

void BuildTray::on_actionQuit_triggered()
{
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
    else
        event->accept();
}

void BuildTray::trayInit()
{
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
