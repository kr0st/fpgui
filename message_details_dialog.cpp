#include "message_details_dialog.h"
#include "ui_message_details_dialog.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>


Message_Details_Dialog::Message_Details_Dialog(QWidget *parent, QString& message) :
    QDialog(parent),
    ui(new Ui::Message_Details_Dialog)
{
    ui->setupUi(this);

    QTableWidget* widget = ui->details_widget;
    widget->clearSelection();
    widget->clearContents();
    widget->setColumnCount(1);

    for (int i = 0; i < 3; ++i)
    {
        widget->insertRow(i);
        widget->setVerticalHeaderItem(i, new QTableWidgetItem("one"));
        widget->setItem(i, 0, new QTableWidgetItem("two"));
    }
}

Message_Details_Dialog::~Message_Details_Dialog()
{
    delete ui;
}

/*
std::vector<std::string> strip_json(const std::string& fields_to_leave, const std::vector<std::string>& json_strings)
{
    std::vector<std::string> out_strings;
    std::vector<std::string> fields;

    std::istringstream is(fields_to_leave);
    std::string part;
    while (std::getline(is, part, ','))
      fields.push_back(part);

    for (const auto& js: json_strings)
    {
        rapidjson::GenericDocument<rapidjson::UTF8<>> js_from, js_to;
        std::unique_ptr<char[]> to_parse(new char[js.size() + 1]);

        memcpy(to_parse.get(), js.c_str(), js.size());
        to_parse.get()[js.size()] = 0;

        js_to.SetObject();
        js_from.ParseInsitu(to_parse.get());

        if (js_from.IsNull())
        {
            qCritical() << "JSON document is invalid!";
            continue;
        }

        if (js_from.IsObject())
        {
            rapidjson::Value::Object jsobj(js_from.GetObject());

            for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
            {
                std::string key(it->name.GetString());
                for (const auto& field: fields)
                    if (key.compare(field) == 0)
                    {
                        js_to.AddMember(it->name, it->value, js_to.GetAllocator());
                        break;
                    }
            }
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        js_to.Accept(writer);
        out_strings.push_back(buffer.GetString());
    }

    return out_strings;
}
*/
