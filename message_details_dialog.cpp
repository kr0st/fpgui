#include "message_details_dialog.h"
#include "ui_message_details_dialog.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <QtDebug>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>

#include <utils.h>


Message_Details_Dialog::Message_Details_Dialog(QWidget *parent, QString& message):
QDialog(parent),
ui(new Ui::Message_Details_Dialog)
{
    file_base64_ = "";
    ui->setupUi(this);

    QTableWidget* widget = ui->details_widget;
    widget->clearSelection();
    widget->clearContents();
    widget->setColumnCount(1);
    widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->save_button->setEnabled(false);

    std::string js(message.toStdString());

    rapidjson::GenericDocument<rapidjson::UTF8<>> js_from;
    std::unique_ptr<char[]> to_parse(new char[js.size() + 1]);

    memcpy(to_parse.get(), js.c_str(), js.size());
    to_parse.get()[js.size()] = 0;

    js_from.ParseInsitu(to_parse.get());

    if (js_from.IsNull())
    {
        qCritical() << "JSON document is invalid! Cannot get log message details.";
        return;
    }

    if (js_from.IsObject())
    {
        rapidjson::Value::Object jsobj(js_from.GetObject());

        int i = 0;
        bool file_found = false;

        for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
        {
            std::string key(it->name.GetString());
            if (key.compare("file") == 0)
            {
                file_found = true;
                ui->save_button->setEnabled(true);
            }

            if (file_found)
                if (key.compare("text") == 0)
                {
                    file_base64_ = it->value.GetString();
                    continue;
                }

            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

            it->value.Accept(writer);
            std::string value(buffer.GetString());

            if (it->value.GetType() == rapidjson::Type::kTrueType)
                value = "True";
            if (it->value.GetType() == rapidjson::Type::kFalseType)
                value = "False";

            if (it->value.IsInt64())
            {
                long long num = it->value.GetInt64();
                value = std::to_string(num);
            }
            else
                if (it->value.IsDouble())
                {
                    double num = it->value.GetDouble();
                    value = std::to_string(num);
                }

            if (it->value.GetType() == rapidjson::Type::kStringType)
                value = it->value.GetString();

            widget->insertRow(i);
            widget->setVerticalHeaderItem(i, new QTableWidgetItem(key.c_str()));
            widget->setItem(i, 0, new QTableWidgetItem(value.c_str()));

            i++;
        }
    }

    ui->details_widget->addAction(ui->actionCopy);
}

Message_Details_Dialog::~Message_Details_Dialog()
{
    delete ui;
}

void Message_Details_Dialog::on_actionCopy_triggered()
{
    if (ui->details_widget->selectedItems().size() > 0)
    {
        QTableWidgetItem* item = ui->details_widget->selectedItems()[0];
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(item->text());
    }
}

void Message_Details_Dialog::on_save_button_clicked()
{
    if (file_base64_.empty())
        return;

    QFileDialog dialog(this, "Select Folder to Save Files into", QDir::homePath());
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if ((dialog.exec() == QDialog::Rejected) || (dialog.selectedFiles().empty()))
        return;

    generic_utils::ui::message_box(dialog.selectedFiles()[0]);
}