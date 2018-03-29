#ifndef HISTORYBROWSERWINDOW_H
#define HISTORYBROWSERWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <table_view.h>
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

        Ui::HistoryBrowserWindow *ui;
        fpgui::ui::Table_View* table_view_;

        void resizeEvent(QResizeEvent* event);


    protected:

      //void closeEvent(QCloseEvent *event) { event->ignore(); hide(); }
};

#endif // HISTORYBROWSERWINDOW_H
