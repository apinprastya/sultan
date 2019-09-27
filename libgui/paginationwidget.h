#ifndef PAGINATIONWIDGET_H
#define PAGINATIONWIDGET_H

#include "gui_global.h"
#include <QWidget>

namespace Ui {
class PaginationWidget;
}

namespace LibGUI {

class GUISHARED_EXPORT PaginationWidget : public QWidget
{
    Q_OBJECT

public:
    PaginationWidget(QWidget *parent = nullptr);
    ~PaginationWidget();
    void setCurrentPerPage(int index, bool blockSignal);

private:
    Ui::PaginationWidget *ui;
    int mMaxPage;

signals:
    void pageChanged(int page);
    void perPageChanged(int value);

private slots:
    void spinChanged(int value);
    void comboIndexChanged();

public slots:
    void setMaxPage(int value);
    void setCurrentPage(int value);
};

}
#endif // PAGINATIONWIDGET_H
