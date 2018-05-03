#include "key_press_emitter.h"

Key_Press_Emitter::Key_Press_Emitter()
{
}

bool Key_Press_Emitter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent keyEvent(*static_cast<QKeyEvent *>(event));
        emit key_pressed(keyEvent);
    }

    return QObject::eventFilter(obj, event);
}
