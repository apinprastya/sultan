#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "global_constant.h"

using namespace LibGUI;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setModal(true);
    ui->textBrowser->setOpenExternalLinks(true);
    ui->labelTitle->setText(LibG::CONSTANT::ABOUT_APP_NAME.arg(qApp->applicationVersion()));
    ui->textBrowser->setText(QStringLiteral("<center>Sultan POS is simple POS software \
                    <br><br>Project can be found at <br><a href=\"https://github.com/apinprastya/sultan\"> \
                    https://github.com/apinprastya/sultan</a><br><br>\
                    Some icons are licenced under Creative Commons Attribution 3.0 License. And downloaded from <br> \
                    <a href=\"https://github.com/yusukekamiyamane/fugue-icons\"> \
                    https://github.com/yusukekamiyamane/fugue-icons</a></center>"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
