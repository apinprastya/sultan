#ifndef DOUBLESPINBOXDELEGATE_H
#define DOUBLESPINBOXDELEGATE_H

#include "gui_global.h"
#include <QStyledItemDelegate>

namespace LibGUI {

class GUISHARED_EXPORT DoubleSpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DoubleSpinBoxDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
    inline void setStep(float step) { mStep = step; }
    inline int getStep() { return mStep; }
    inline void setMax(float max) { mMax = max; }
    inline int getMax() { return mMax; }
    inline void setMin(float min) { mMin = min; }
    inline int getMin() { return mMin; }

private:
    float mStep;
    float mMax;
    float mMin;
};

}

#endif // DOUBLESPINBOXDELEGATE_H
