#ifndef HISTORYBROWSERWINDOW_H
#define HISTORYBROWSERWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <history_browser_view.h>
#include <windowwithmessagebox.h>
#include <key_press_emitter.h>

namespace Ui {
class HistoryBrowserWindow;
}

namespace fpgui { namespace ui {

class HistoryBrowserWindow : public QWidget, public fpgui::ui::WindowWithMessageBoxInterface
{
    Q_OBJECT
    Q_INTERFACES(fpgui::ui::WindowWithMessageBoxInterface)

    public:

        explicit HistoryBrowserWindow(QWidget *parent = nullptr);
        ~HistoryBrowserWindow();
        void inject_table_view(fpgui::ui::History_Browser_View* history_browser_view) { history_browser_view_ = history_browser_view; }
        void on_page_counter_update(int current_page, int total_pages);
        int get_per_page_count();


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

        void on_from_datetime_editingFinished();
        void on_to_datetime_editingFinished();

        void on_left_button_clicked();
        void on_right_button_clicked();
        void on_per_page_edit_editingFinished();
        void on_goto_edit_editingFinished();

        void on_tableWidget_itemActivated(QTableWidgetItem *item);
        void on_key_press(QKeyEvent e);


        void on_actionCopy_triggered();

private:

        Ui::HistoryBrowserWindow *ui;
        fpgui::ui::History_Browser_View* history_browser_view_;
        Key_Press_Emitter key_emitter_;

        void resizeEvent(QResizeEvent* event);
};

}};

#endif // HISTORYBROWSERWINDOW_H
