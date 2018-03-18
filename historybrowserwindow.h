#ifndef HISTORYBROWSERWINDOW_H
#define HISTORYBROWSERWINDOW_H

#include <QWidget>

namespace Ui {
class HistoryBrowserWindow;
}

class HistoryBrowserWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryBrowserWindow(QWidget *parent = 0);
    ~HistoryBrowserWindow();

private:
    Ui::HistoryBrowserWindow *ui;
};

#endif // HISTORYBROWSERWINDOW_H
