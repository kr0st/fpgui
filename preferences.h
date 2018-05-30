#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

private slots:
    void on_button_config_tabs_clicked();

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
