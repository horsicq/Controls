#include "dialogbits8.h"
#include "ui_dialogbits8.h"

DialogBits8::DialogBits8(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBits8)
{
    ui->setupUi(this);

    g_nBits = 8;
    g_nValue = 0;
    g_bIsReadonly = true;

    _handleButton(ui->toolButton0);
    _handleButton(ui->toolButton1);
    _handleButton(ui->toolButton2);
    _handleButton(ui->toolButton3);
    _handleButton(ui->toolButton4);
    _handleButton(ui->toolButton5);
    _handleButton(ui->toolButton6);
    _handleButton(ui->toolButton7);
}

DialogBits8::~DialogBits8()
{
    delete ui;
}

void DialogBits8::setData8(quint8 nValue, bool bIsReadonly)
{
    g_nBits = 8;
    g_nValue = nValue;
    g_bIsReadonly = bIsReadonly;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_8);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_8);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_8);

    ui->lineEditHex->setValidator(&g_validatorHex);
    ui->lineEditSigned->setValidator(&g_validatorSigned);
    ui->lineEditUnsigned->setValidator(&g_validatorUnsigned);

    ui->pushButtonOK->setEnabled(!g_bIsReadonly);

    ui->pushButtonCancel->setFocus();

    reload();
}

void DialogBits8::reload()
{
    enableControls(false);

    if (g_nBits == 8) {
        if (!(ui->lineEditHex->hasFocus())) {
            ui->lineEditHex->setText(QString::number((quint8)g_nValue, 16));
        }
        if (!(ui->lineEditSigned->hasFocus())) {
            ui->lineEditSigned->setText(QString::number((qint8)g_nValue, 10));
        }
        if (!(ui->lineEditUnsigned->hasFocus())) {
            ui->lineEditUnsigned->setText(QString::number((quint8)g_nValue, 10));
        }
    }

    for (qint32 i = 0; i < g_nBits; i++) {
        if (!g_listButtons.at(i)->hasFocus()) {
            bool bIsChecked = g_nValue & ((quint64)1 << i);
            g_listButtons.at(i)->setChecked(bIsChecked);

            if (bIsChecked) {
                g_listButtons.at(i)->setText("1");
            } else {
                g_listButtons.at(i)->setText("0");
            }
        }
    }

    enableControls(true);
}

void DialogBits8::_handleButton(QToolButton *pToolButton)
{
    g_listButtons.append(pToolButton);
    connect(pToolButton, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
}

void DialogBits8::on_lineEditHex_textChanged(const QString &sString)
{
    g_nValue = sString.toUInt(0, 16);
    reload();
}

void DialogBits8::on_lineEditSigned_textChanged(const QString &sString)
{
    g_nValue = sString.toInt(0, 10);
    reload();
}

void DialogBits8::on_lineEditUnsigned_textChanged(const QString &sString)
{
    g_nValue = sString.toUInt(0, 10);
    reload();
}

void DialogBits8::toggledSlot(bool bState)
{
    Q_UNUSED(bState)

    ui->pushButtonCancel->setFocus();

    for (qint32 i = 0; i < g_nBits; i++) {
        bool bBit = g_listButtons.at(i)->isChecked();
        quint64 nFF = 0x01;
        quint64 _nValue = (quint64)bBit;
        nFF = nFF << i;
        _nValue = _nValue << i;
        g_nValue = g_nValue & (~nFF);
        g_nValue = g_nValue | _nValue;
    }

    reload();
}

void DialogBits8::enableControls(bool bState)
{
    ui->lineEditHex->blockSignals(!bState);
    ui->lineEditSigned->blockSignals(!bState);
    ui->lineEditUnsigned->blockSignals(!bState);

    qint32 nNumberOfRecords = g_listButtons.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        g_listButtons.at(i)->blockSignals(!bState);
    }
}
