// ButtonModified.h

#ifndef BUTTONMODIFIED_H
#define BUTTONMODIFIED_H

#include <QPushButton>
#include <QMouseEvent>

class ButtonModified : public QPushButton {
    Q_OBJECT

public:
    // Constructor inherits from QPushButton
    using QPushButton::QPushButton;

protected:
    // Override mousePressEvent to emit rightClicked signal on right-clicks
    void mousePressEvent(QMouseEvent *event) override;

signals:
    // Custom signal for right-click events
    void rightClicked();
};

#endif // BUTTONMODIFIED_H
