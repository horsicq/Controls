/* Copyright (c) 2023-2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dialogbits8.h"
#include "ui_dialogbits8.h"

DialogBits8::DialogBits8(QWidget *parent) : QDialog(parent), ui(new Ui::DialogBits8)
{
    ui->setupUi(this);

    g_nBits = 8;
    g_nValue = 0;
    g_bIsReadonly = true;

    ui->lineEditHex->setValidator(&g_validatorHex);
    ui->lineEditSigned->setValidator(&g_validatorSigned);
    ui->lineEditUnsigned->setValidator(&g_validatorUnsigned);
    ui->lineEditBin->setValidator(&g_validatorBin);

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
    _handleButton(ui->toolButton16);
    _handleButton(ui->toolButton17);
    _handleButton(ui->toolButton18);
    _handleButton(ui->toolButton19);
    _handleButton(ui->toolButton20);
    _handleButton(ui->toolButton21);
    _handleButton(ui->toolButton22);
    _handleButton(ui->toolButton23);
    _handleButton(ui->toolButton24);
    _handleButton(ui->toolButton25);
    _handleButton(ui->toolButton26);
    _handleButton(ui->toolButton27);
    _handleButton(ui->toolButton28);
    _handleButton(ui->toolButton29);
    _handleButton(ui->toolButton30);
    _handleButton(ui->toolButton31);
    _handleButton(ui->toolButton32);
    _handleButton(ui->toolButton33);
    _handleButton(ui->toolButton34);
    _handleButton(ui->toolButton35);
    _handleButton(ui->toolButton36);
    _handleButton(ui->toolButton37);
    _handleButton(ui->toolButton38);
    _handleButton(ui->toolButton39);
    _handleButton(ui->toolButton40);
    _handleButton(ui->toolButton41);
    _handleButton(ui->toolButton42);
    _handleButton(ui->toolButton43);
    _handleButton(ui->toolButton44);
    _handleButton(ui->toolButton45);
    _handleButton(ui->toolButton46);
    _handleButton(ui->toolButton47);
    _handleButton(ui->toolButton48);
    _handleButton(ui->toolButton49);
    _handleButton(ui->toolButton50);
    _handleButton(ui->toolButton51);
    _handleButton(ui->toolButton52);
    _handleButton(ui->toolButton53);
    _handleButton(ui->toolButton54);
    _handleButton(ui->toolButton55);
    _handleButton(ui->toolButton56);
    _handleButton(ui->toolButton57);
    _handleButton(ui->toolButton58);
    _handleButton(ui->toolButton59);
    _handleButton(ui->toolButton60);
    _handleButton(ui->toolButton61);
    _handleButton(ui->toolButton62);
    _handleButton(ui->toolButton63);

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
    g_validatorBin.setMode(XLineEditValidator::MODE_BIN_8);

    ui->groupBox0_7->show();
    ui->groupBox8_15->hide();
    ui->groupBox16_23->hide();
    ui->groupBox24_31->hide();
    ui->groupBox32_39->hide();
    ui->groupBox40_47->hide();
    ui->groupBox48_55->hide();
    ui->groupBox56_63->hide();

    reload();
}

void DialogBits8::setValue_uint16(quint16 nValue)
{
    g_nBits = 16;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_16);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_16);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_16);
    g_validatorBin.setMode(XLineEditValidator::MODE_BIN_16);

    ui->groupBox0_7->show();
    ui->groupBox8_15->show();
    ui->groupBox16_23->hide();
    ui->groupBox24_31->hide();
    ui->groupBox32_39->hide();
    ui->groupBox40_47->hide();
    ui->groupBox48_55->hide();
    ui->groupBox56_63->hide();

    reload();
}

void DialogBits8::setValue_uint32(quint32 nValue)
{
    g_nBits = 32;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_32);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_32);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_32);
    g_validatorBin.setMode(XLineEditValidator::MODE_BIN_32);

    ui->groupBox0_7->show();
    ui->groupBox8_15->show();
    ui->groupBox16_23->show();
    ui->groupBox24_31->show();
    ui->groupBox32_39->hide();
    ui->groupBox40_47->hide();
    ui->groupBox48_55->hide();
    ui->groupBox56_63->hide();

    reload();
}

void DialogBits8::setValue_uint64(quint64 nValue)
{
    g_nBits = 64;
    g_nValue = nValue;

    g_validatorHex.setMode(XLineEditValidator::MODE_HEX_64);
    g_validatorSigned.setMode(XLineEditValidator::MODE_SIGN_DEC_64);
    g_validatorUnsigned.setMode(XLineEditValidator::MODE_DEC_64);
    g_validatorBin.setMode(XLineEditValidator::MODE_BIN_64);

    ui->groupBox0_7->show();
    ui->groupBox8_15->show();
    ui->groupBox16_23->show();
    ui->groupBox24_31->show();
    ui->groupBox32_39->show();
    ui->groupBox40_47->show();
    ui->groupBox48_55->show();
    ui->groupBox56_63->show();

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
            ui->lineEditSigned->setText(QString::number((qint8)g_nValue, 10));
        } else if (g_nBits == 16) {
            ui->lineEditSigned->setText(QString::number((qint16)g_nValue, 10));
        } else if (g_nBits == 32) {
            ui->lineEditSigned->setText(QString::number((qint32)g_nValue, 10));
        } else if (g_nBits == 64) {
            ui->lineEditSigned->setText(QString::number((qint64)g_nValue, 10));
        }
    }
    if (!(ui->lineEditUnsigned->hasFocus())) {
        if (g_nBits == 8) {
            ui->lineEditUnsigned->setText(QString::number((quint8)g_nValue, 10));
        } else if (g_nBits == 16) {
            ui->lineEditUnsigned->setText(QString::number((quint16)g_nValue, 10));
        } else if (g_nBits == 32) {
            ui->lineEditUnsigned->setText(QString::number((quint32)g_nValue, 10));
        } else if (g_nBits == 64) {
            ui->lineEditUnsigned->setText(QString::number((quint64)g_nValue, 10));
        }
    }
    if (!(ui->lineEditBin->hasFocus())) {
        if (g_nBits == 8) {
            ui->lineEditBin->setText(XLineEditValidator::value8ToBinString((quint8)g_nValue));
        } else if (g_nBits == 16) {
            ui->lineEditBin->setText(XLineEditValidator::value16ToBinString((quint16)g_nValue));
        } else if (g_nBits == 32) {
            ui->lineEditBin->setText(XLineEditValidator::value32ToBinString((quint32)g_nValue));
        } else if (g_nBits == 64) {
            ui->lineEditBin->setText(XLineEditValidator::value64ToBinString((quint64)g_nValue));
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
    g_nValue = sString.toULongLong(0, 16);
    reload();
}

void DialogBits8::on_lineEditSigned_textChanged(const QString &sString)
{
    g_nValue = sString.toLongLong(0, 10);
    reload();
}

void DialogBits8::on_lineEditUnsigned_textChanged(const QString &sString)
{
    g_nValue = sString.toULongLong(0, 10);
    reload();
}

void DialogBits8::on_lineEditBin_textChanged(const QString &sString)
{
    g_nValue = XLineEditValidator::binStringToValue(sString);
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
    ui->lineEditBin->blockSignals(!bState);

    qint32 nNumberOfRecords = g_listButtons.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        g_listButtons.at(i)->blockSignals(!bState);
    }
}

void DialogBits8::on_pushButtonCancel_clicked()
{
    this->close();
}
