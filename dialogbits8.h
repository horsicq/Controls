#ifndef DIALOGBITS8_H
#define DIALOGBITS8_H

#include <QDialog>
#include <QToolButton>
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

    void setData8(quint8 nValue, bool bIsReadonly);
    void reload();

private slots:
    void _handleButton(QToolButton *pToolButton);
    void on_lineEditHex_textChanged(const QString &sString);
    void on_lineEditSigned_textChanged(const QString &sString);
    void on_lineEditUnsigned_textChanged(const QString &sString);
    void toggledSlot(bool bState);
    void enableControls(bool bState);

private:
    Ui::DialogBits8 *ui;
    quint64 g_nValue;
    bool g_bIsReadonly;
    XLineEditValidator g_validatorHex;
    XLineEditValidator g_validatorSigned;
    XLineEditValidator g_validatorUnsigned;
    QList<QToolButton *> g_listButtons;
    qint32 g_nBits;
};

#endif // DIALOGBITS8_H
