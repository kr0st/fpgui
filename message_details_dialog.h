#ifndef MESSAGE_DETAILS_DIALOG_H
#define MESSAGE_DETAILS_DIALOG_H

#include <QDialog>

namespace Ui {
class Message_Details_Dialog;
}

class Message_Details_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Message_Details_Dialog(QWidget *parent = 0);
    ~Message_Details_Dialog();

private:
    Ui::Message_Details_Dialog *ui;
};

#endif // MESSAGE_DETAILS_DIALOG_H
