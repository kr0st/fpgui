#ifndef MESSAGE_DETAILS_DIALOG_H
#define MESSAGE_DETAILS_DIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class Message_Details_Dialog;
}

class Message_Details_Dialog : public QDialog
{
    Q_OBJECT

    public:

        explicit Message_Details_Dialog(QWidget *parent, QString& message);
        ~Message_Details_Dialog();


private slots:

        void on_actionCopy_triggered();
        void on_save_button_clicked();

private:

        Ui::Message_Details_Dialog *ui;
        std::string file_base64_;
};

#endif // MESSAGE_DETAILS_DIALOG_H
