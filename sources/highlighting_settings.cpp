#include <QPalette>
#include <QColorDialog>
#include <QGridLayout>

#include "utils.h"

#include "highlighting_settings.h"
#include "ui_highlighting_settings.h"


void Controls_Quadruple::connect_signals()
{
    connect(color_button, SIGNAL(clicked(bool)), SLOT(on_color_button_clicked()));
    connect(bold_check, SIGNAL(stateChanged(int)), SLOT(on_bold_check_state_changed(int)));
}

void Controls_Quadruple::on_color_button_clicked()
{
    QColorDialog color_selection(color_button->parentWidget());
    QColor col = color_selection.getColor(color);

    if (col.isValid())
        color = col;

    QString colorname = color.name();

    if (bold_check->isChecked())
        color_button->setStyleSheet("color: " + colorname + "; " + "font-weight: bold;");
    else
        color_button->setStyleSheet("color: " + colorname + "; " + "font-weight: normal;");
}

void Controls_Quadruple::on_bold_check_state_changed(int state)
{
    QString colorname = color.name();

    if (state == Qt::Checked)
    {
        color_button->setStyleSheet("color: " + colorname + "; " + "font-weight: bold;");
    }
    else
    {
        color_button->setStyleSheet("color: " + colorname + "; " + "font-weight: normal;");
    }

    color_button->update();
}

static std::vector<Controls_Quadruple*> g_control_quads;

Highlighting_Settings::Highlighting_Settings(fpgui::settings::App_Configuration& app_config, QWidget *parent):
QDialog(parent),
ui(new Ui::Highlighting_Settings),
app_config_(app_config)
{
    ui->setupUi(this);
    ui->check_differential->setCheckState(app_config_.highlighting.diff_enabled ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    QString colorname = app_config_.highlighting.base_color.name();
    ui->button_base_color->setStyleSheet("background-color: " + colorname + ";");

    base_color_ = app_config_.highlighting.base_color;
    Controls_Quadruple* quad = new Controls_Quadruple();

    quad->bold_check = ui->check_bold;
    quad->color_button = ui->button_text_color;
    quad->name_edit = ui->edit_field;
    quad->value_edit = ui->edit_value;
    quad->connect_signals();

    g_control_quads.push_back(quad);

    ui->check_highlight->setCheckState(app_config_.highlighting.value_based_enabled ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    bool first = true;
    for (auto item : app_config_.highlighting.config)
    {
        if (first)
            first = false;
        else
            on_button_add_clicked();

        g_control_quads.back()->color = item.color;
        g_control_quads.back()->name_edit->setText(item.field.c_str());
        g_control_quads.back()->value_edit->setText(item.value.c_str());
        g_control_quads.back()->bold_check->setCheckState(item.bold ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        g_control_quads.back()->on_bold_check_state_changed(item.bold ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
}

Highlighting_Settings::~Highlighting_Settings()
{
    for (auto quad: g_control_quads)
        delete quad;

    g_control_quads.clear();

    delete ui;
}

void Highlighting_Settings::on_Highlighting_Settings_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        app_config_.highlighting.diff_enabled = (ui->check_differential->checkState() == Qt::CheckState::Checked ? true : false);
        app_config_.highlighting.base_color = base_color_;

        if (ui->check_highlight->checkState() == Qt::CheckState::Checked)
        {
            app_config_.highlighting.config.clear();
            app_config_.highlighting.value_based_enabled = true;
            for (Controls_Quadruple* quad : g_control_quads)
            {
                fpgui::settings::Highlighting_Configuration::Config_Item item;

                item.bold = (quad->bold_check->checkState() == Qt::CheckState::Checked);
                item.color = quad->color;
                item.field = quad->name_edit->text().toStdString();
                item.value = quad->value_edit->text().toStdString();

                app_config_.highlighting.config.push_back(item);
            }
        }
        else
            app_config_.highlighting.value_based_enabled = false;
    }
}

void Highlighting_Settings::on_button_base_color_clicked()
{
    QColorDialog color_selection(this);
    QColor col = color_selection.getColor(base_color_);

    if (col.isValid())
        base_color_ = col;

    QString colorname = base_color_.name();
    ui->button_base_color->setStyleSheet("background-color: " + colorname + ";");
}

void Highlighting_Settings::on_button_add_clicked()
{
    QGridLayout* gridLayout(static_cast<QGridLayout*>(ui->group_valuebased->layout()));

    QRect box(ui->group_valuebased->geometry());
    box.setHeight(box.height() + 35);

    ui->group_valuebased->setGeometry(box);

    int rows = (gridLayout->count() - 2) / (gridLayout->columnCount() - 1);
    int count = gridLayout->count();

    if (count == 6)
        rows = 2;
    else
        rows += 2;

    Controls_Quadruple* quad = new Controls_Quadruple();

    quad->name_edit = new QLineEdit(tr("field name"), this);
    quad->value_edit = new QLineEdit(tr("field value"), this);
    quad->color_button = new QPushButton(tr("text color"), this);
    quad->bold_check = new QCheckBox(tr("bold text"), this);

    gridLayout->addWidget(quad->name_edit, rows, 1);
    gridLayout->addWidget(quad->value_edit, rows, 2);
    gridLayout->addWidget(quad->color_button, rows, 3);
    gridLayout->addWidget(quad->bold_check, rows, 4);

    quad->connect_signals();
    g_control_quads.push_back(quad);

    ui->gridLayout->update();
}

void Highlighting_Settings::on_button_remove_clicked()
{
    QGridLayout* gridLayout(static_cast<QGridLayout*>(ui->group_valuebased->layout()));

    int rows = (gridLayout->count() - 2) / (gridLayout->columnCount() - 1);
    int count = gridLayout->count();

    auto sz = g_control_quads.size();

    auto control_remover = [&sz]()
    {
        delete g_control_quads[sz - 1];
        g_control_quads[sz - 1] = nullptr;
        g_control_quads.resize(sz - 1);
    };

    if (count == 6)
    {
        if (sz > 0) control_remover();

        ui->check_bold->setCheckState(Qt::Unchecked);
        ui->button_text_color->setStyleSheet("color: black; font-weight: normal;");
        ui->edit_field->setText(tr("field name"));
        ui->edit_value->setText(tr("field value"));

        return;
    }
    else
        rows += 2;

    if (count == 10)
        rows = 3;

    QRect box(ui->group_valuebased->geometry());
    box.setHeight(box.height() - 35);

    for (int i = 1; i <= 4; ++i)
    {
        QLayoutItem* item = gridLayout->itemAtPosition(rows - 1, i);

        gridLayout->removeItem(item);
        gridLayout->removeWidget(item->widget());

        delete item->widget();
        delete item;
    }

    control_remover();

    ui->group_valuebased->setGeometry(box);
    gridLayout->invalidate();
    ui->group_valuebased->update();
}
