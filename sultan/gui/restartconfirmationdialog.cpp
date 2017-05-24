#include "restartconfirmationdialog.h"
#include "ui_restartconfirmationdialog.h"
#include <QProcess>

RestartConfirmationDialog::RestartConfirmationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RestartConfirmationDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    connect(ui->pushRestart, SIGNAL(clicked(bool)), SLOT(restartClicked()));
    connect(ui->pushExit, SIGNAL(clicked(bool)), SLOT(exitClicked()));
}

RestartConfirmationDialog::~RestartConfirmationDialog()
{
    delete ui;
}

void RestartConfirmationDialog::setMessage(const QString &title, const QString &msg)
{
    setWindowTitle(title);
    ui->labelTitle->setText(msg);
}

void RestartConfirmationDialog::restartClicked()
{
    qApp->quit();
    QStringList list;
    const QStringList &args = qApp->arguments();
    for(int i = 0; i < args.count(); i++) {
        if(i == 0) continue;
        list.append(args[i]);
    }
    QProcess::startDetached(qApp->arguments()[0], list);
}

void RestartConfirmationDialog::exitClicked()
{
    qApp->quit();
}
