#include "message_details_dialog.h"
#include "ui_message_details_dialog.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <QtDebug>


Message_Details_Dialog::Message_Details_Dialog(QWidget *parent, QString& message):
QDialog(parent),
ui(new Ui::Message_Details_Dialog)
{
    ui->setupUi(this);

    QTableWidget* widget = ui->details_widget;
    widget->clearSelection();
    widget->clearContents();
    widget->setColumnCount(1);
    widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

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
        for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
        {
            std::string key(it->name.GetString());

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
}

Message_Details_Dialog::~Message_Details_Dialog()
{
    delete ui;
}
