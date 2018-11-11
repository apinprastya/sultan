#include "doublespinboxdelegate.h"
#include <QDoubleSpinBox>
#include <QDebug>

using namespace LibGUI;

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      mStep(-1000000),
      mMax(1000000),
      mMin(0)
{
}

QWidget *DoubleSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(mMin);
    editor->setMaximum(mMax);
    editor->setSingleStep(mStep);
    return editor;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::DisplayRole).toDouble();
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    float value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
editor->setGeometry(option.rect);
}
