#ifndef HISTORYBROWSERWINDOW_H
#define HISTORYBROWSERWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <history_browser_view.h>
#include <windowwithmessagebox.h>

namespace Ui {
class HistoryBrowserWindow;
}

class HistoryBrowserWindow : public QWidget, public WindowWithMessageBoxInterface
{
    Q_OBJECT
    Q_INTERFACES(WindowWithMessageBoxInterface)

    public:

        explicit HistoryBrowserWindow(QWidget *parent = 0);
        ~HistoryBrowserWindow();
        void inject_table_view(fpgui::ui::History_Browser_View* history_browser_view) { history_browser_view_ = history_browser_view; }


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

private:

        Ui::HistoryBrowserWindow *ui;
        fpgui::ui::History_Browser_View* history_browser_view_;

        void resizeEvent(QResizeEvent* event);
};

#endif // HISTORYBROWSERWINDOW_H
