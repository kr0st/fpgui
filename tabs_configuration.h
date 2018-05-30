#ifndef TABS_CONFIGURATION_H
#define TABS_CONFIGURATION_H

#include <QDialog>

namespace Ui {
class Tabs_Configuration;
}

class Tabs_Configuration : public QDialog
{
    Q_OBJECT

public:
    explicit Tabs_Configuration(QWidget *parent = 0);
    ~Tabs_Configuration();

private:
    Ui::Tabs_Configuration *ui;
};

#endif // TABS_CONFIGURATION_H
