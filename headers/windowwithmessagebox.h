#ifndef WINDOWWITHMESSAGEBOX_H
#define WINDOWWITHMESSAGEBOX_H

#include <QObject>

namespace fpgui { namespace ui {

class WindowWithMessageBoxInterface
{
    public:

        virtual void show_hide(){}
        virtual ~WindowWithMessageBoxInterface(){}


    public slots:

        virtual void message_box(const QString &text) = 0;


    signals:

        virtual void display_message(const QString &text) = 0;
};

}};

Q_DECLARE_INTERFACE(fpgui::ui::WindowWithMessageBoxInterface, "WindowWithMessageBoxInterface")

#endif // WINDOWWITHMESSAGEBOX_H
