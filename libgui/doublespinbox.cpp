#include "doublespinbox.h"
#include <QDebug>
#include <QFocusEvent>
#include <QTimer>

namespace LibGUI {

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent) {}

void DoubleSpinBox::focusInEvent(QFocusEvent *event) {
    QDoubleSpinBox::focusInEvent(event);
    if (event->gotFocus()) {
        QTimer::singleShot(10, this, SLOT(selectAll()));
    }
}

} // namespace LibGUI
