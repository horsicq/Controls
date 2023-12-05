#ifndef DIALOGBITS8_H
#define DIALOGBITS8_H

#include <QDialog>
#include "xlineeditvalidator.h"

namespace Ui {
class DialogBits8;
}

class DialogBits8 : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBits8(QWidget *parent = nullptr);
    ~DialogBits8();

    void setData(quint8 nValue, bool bIsReadonly);
    void reload();

private:
    Ui::DialogBits8 *ui;
    quint8 g_nValue;
    bool g_bIsReadonly;
};

#endif // DIALOGBITS8_H
