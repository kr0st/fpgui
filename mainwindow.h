#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <table_view.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        void inject_table_view(fpgui::ui::Table_View* table_view) { table_view_ = table_view; }


    private:

        Ui::MainWindow* ui;
        fpgui::ui::Table_View* table_view_;

        void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
