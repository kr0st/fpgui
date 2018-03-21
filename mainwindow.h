#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <table_view.h>
#include <windowwithmessagebox.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public WindowWithMessageBoxInterface
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        void inject_table_view(fpgui::ui::Table_View* table_view) { table_view_ = table_view; }


    signals:

        void display_message(const QString &text);


    public slots:

        void message_box(const QString &text);


    private slots:

        void on_autoscroll_box_stateChanged(int state);
        void on_sorting_box_stateChanged(int state);

        void on_clear_button_clicked();
        void on_connection_button_clicked();


        void on_quickfilter_edit_textEdited(const QString &text);


    private:

        Ui::MainWindow* ui;
        fpgui::ui::Table_View* table_view_;

        void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
