// ButtonModified.cpp

#include "ButtonModified.h"

void ButtonModified::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        // Emit the custom rightClicked signal when the right mouse button is pressed
        emit rightClicked();
    } else {
        // Call the base class implementation for other types of mouse button presses
        QPushButton::mousePressEvent(event);
    }
}
