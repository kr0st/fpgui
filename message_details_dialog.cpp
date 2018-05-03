#include "message_details_dialog.h"
#include "ui_message_details_dialog.h"

Message_Details_Dialog::Message_Details_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Message_Details_Dialog)
{
    ui->setupUi(this);
}

Message_Details_Dialog::~Message_Details_Dialog()
{
    delete ui;
}
