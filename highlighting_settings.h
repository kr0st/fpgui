#ifndef HIGHLIGHTING_SETTINGS_H
#define HIGHLIGHTING_SETTINGS_H

#include <QDialog>

namespace Ui {
class Highlighting_Settings;
}

class Highlighting_Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Highlighting_Settings(QWidget *parent = 0);
    ~Highlighting_Settings();

private:
    Ui::Highlighting_Settings *ui;
};

#endif // HIGHLIGHTING_SETTINGS_H
