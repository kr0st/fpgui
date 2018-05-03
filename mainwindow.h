#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <table_view.h>
#include <windowwithmessagebox.h>
#include <history_browser_view.h>
#include <key_press_emitter.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public WindowWithMessageBoxInterface
{
    Q_OBJECT
    Q_INTERFACES(WindowWithMessageBoxInterface)

    public:

        Ui::MainWindow* ui;
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        void inject_table_view(fpgui::ui::Table_View* table_view) { table_view_ = table_view; }
        void inject_history_browser_view(fpgui::ui::History_Browser_View* history_browser_view){ history_browser_view_ = history_browser_view; }


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
        void on_actionBrowse_triggered();
        void on_tableWidget_itemActivated(QTableWidgetItem *item);

        void on_key_press(QKeyEvent e);


private:

        fpgui::ui::Table_View* table_view_;
        fpgui::ui::History_Browser_View* history_browser_view_;

        Key_Press_Emitter key_emitter_;

        void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
