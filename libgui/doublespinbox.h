#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

namespace LibGUI {

class DoubleSpinBox : public QDoubleSpinBox
{
  public:
    DoubleSpinBox(QWidget *parent = nullptr);

    void focusInEvent(QFocusEvent *event) override;
};

} // namespace LibGUI

#endif // DOUBLESPINBOX_H
