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

    ui->lineEditHex->setValidator(&g_validatorHex);
    ui->lineEditSigned->setValidator(&g_validatorSigned);
    ui->lineEditUnsigned->setValidator(&g_validatorUnsigned);

    _handleButton(ui->toolButton0);
    _handleButton(ui->toolButton1);
    _handleButton(ui->toolButton2);
    _handleButton(ui->toolButton3);
    _handleButton(ui->toolButton4);
    _handleButton(ui->toolButton5);
    _handleButton(ui->toolButton6);
    _handleButton(ui->toolButton7);
    _handleButton(ui->toolButton8);
    _handleButton(ui->toolButton9);
    _handleButton(ui->toolButton10);
    _handleButton(ui->toolButton11);
    _handleButton(ui->toolButton12);
    _handleButton(ui->toolButton13);
    _handleButton(ui->toolButton14);
    _handleButton(ui->toolButton15);

    ui->pushButtonCancel->setFocus();
}

DialogBits8::~DialogBits8()
{
    delete ui;
}

void DialogBits8::setReadonly(bool bIsReadonly)
{
    g_bIsReadonly = bIsReadonly;
    ui->pushButtonOK->setEnabled(!g_bIsReadonly);
}

void DialogBits8::setValue_uint8(quint8 nValue)
{
    g_nBits = 8;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_8);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_8);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_8);

    ui->groupBox0_7->show();
    ui->groupBox8_15->hide();

    reload();
}

void DialogBits8::setValue_uint16(quint16 nValue)
{
    g_nBits = 16;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_16);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_16);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_16);

    ui->groupBox0_7->show();
    ui->groupBox8_15->show();

    reload();
}

void DialogBits8::setValue_uint32(quint32 nValue)
{
    g_nBits = 32;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_32);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_32);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_32);

    reload();
}

void DialogBits8::setValue_uint64(quint64 nValue)
{
    g_nBits = 64;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_64);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_64);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_64);

    reload();
}

void DialogBits8::reload()
{
    enableControls(false);


    if (!(ui->lineEditHex->hasFocus())) {
        if (g_nBits == 8) {
            ui->lineEditHex->setText(QString::number((quint8)g_nValue, 16));
        } else if (g_nBits == 16) {
            ui->lineEditHex->setText(QString::number((quint16)g_nValue, 16));
        } else if (g_nBits == 32) {
            ui->lineEditHex->setText(QString::number((quint32)g_nValue, 16));
        } else if (g_nBits == 64) {
            ui->lineEditHex->setText(QString::number((quint64)g_nValue, 16));
        }
    }
    if (!(ui->lineEditSigned->hasFocus())) {
        if (g_nBits == 8) {
            ui->lineEditSigned->setText(QString::number((qint8)g_nValue, 16));
        } else if (g_nBits == 16) {
            ui->lineEditSigned->setText(QString::number((qint16)g_nValue, 16));
        } else if (g_nBits == 32) {
            ui->lineEditSigned->setText(QString::number((qint32)g_nValue, 16));
        } else if (g_nBits == 64) {
            ui->lineEditSigned->setText(QString::number((qint64)g_nValue, 16));
        }
    }
    if (!(ui->lineEditUnsigned->hasFocus())) {
        if (g_nBits == 8) {
            ui->lineEditUnsigned->setText(QString::number((quint8)g_nValue, 16));
        } else if (g_nBits == 16) {
            ui->lineEditUnsigned->setText(QString::number((quint16)g_nValue, 16));
        } else if (g_nBits == 32) {
            ui->lineEditUnsigned->setText(QString::number((quint32)g_nValue, 16));
        } else if (g_nBits == 64) {
            ui->lineEditUnsigned->setText(QString::number((quint64)g_nValue, 16));
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

quint8 DialogBits8::getValue_uint8()
{
    return (quint8)g_nValue;
}

quint16 DialogBits8::getValue_uint16()
{
    return (quint16)g_nValue;
}

quint32 DialogBits8::getValue_uint32()
{
    return (quint32)g_nValue;
}

quint64 DialogBits8::getValue_uint64()
{
    return g_nValue;
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
