#ifndef KEY_PRESS_EMITTER_H
#define KEY_PRESS_EMITTER_H

#include <QObject>
#include <QKeyEvent>

namespace fpgui { namespace ui {

class Key_Press_Emitter: public QObject
{
    Q_OBJECT

    signals:

        void key_pressed(QKeyEvent e);


    public:

        Key_Press_Emitter();


    protected:

        bool eventFilter(QObject *obj, QEvent *event);
};

}};

#endif // KEY_PRESS_EMITTER_H
