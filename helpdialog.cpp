#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    ui->label_2->setTextFormat(Qt::RichText);
    ui->label_2->setOpenExternalLinks(true);
    ui->label_3->setTextFormat(Qt::RichText);
    ui->label_3->setOpenExternalLinks(true);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::on_OKButton_clicked()
{
    this->close();
}
