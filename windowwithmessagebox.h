#ifndef WINDOWWITHMESSAGEBOX_H
#define WINDOWWITHMESSAGEBOX_H

#include <QObject>

class WindowWithMessageBoxInterface
{
    public:

        virtual void show_hide(){}


    public slots:

        virtual void message_box(const QString &text) = 0;


    signals:

        virtual void display_message(const QString &text) = 0;
};

Q_DECLARE_INTERFACE(WindowWithMessageBoxInterface, "WindowWithMessageBoxInterface")

#endif // WINDOWWITHMESSAGEBOX_H
