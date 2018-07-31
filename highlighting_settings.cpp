#include <QPalette>
#include <QColorDialog>

#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>

#include "highlighting_settings.h"
#include "ui_highlighting_settings.h"

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
}

Highlighting_Settings::~Highlighting_Settings()
{
    delete ui;
}

void Highlighting_Settings::on_Highlighting_Settings_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        app_config_.highlighting.diff_enabled = (ui->check_differential->checkState() == Qt::CheckState::Checked ? true : false);
        app_config_.highlighting.base_color = base_color_;
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
    QGridLayout* gridLayout((QGridLayout*)ui->group_valuebased->layout());

    QRect box(ui->group_valuebased->geometry());
    box.setHeight(box.height() + 35);

    ui->group_valuebased->setGeometry(box);

    int rows = (gridLayout->count() - 2) / (gridLayout->columnCount() - 1);
    int count = gridLayout->count();

    if (count == 6)
        rows = 2;
    else
        rows += 2;

    gridLayout->addWidget(new QLineEdit(tr("field name"), this), rows, 1);
    gridLayout->addWidget(new QLineEdit(tr("field value"), this), rows, 2);
    gridLayout->addWidget(new QPushButton(tr("text color"), this), rows, 3);
    gridLayout->addWidget(new QCheckBox(tr("bold text"), this), rows, 4);

    ui->gridLayout->update();
}

void Highlighting_Settings::on_button_remove_clicked()
{
    QGridLayout* gridLayout((QGridLayout*)ui->group_valuebased->layout());

    int rows = (gridLayout->count() - 2) / (gridLayout->columnCount() - 1);
    int count = gridLayout->count();

    if (count == 6)
        return;
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

    ui->group_valuebased->setGeometry(box);
    gridLayout->invalidate();
    ui->group_valuebased->update();
}
