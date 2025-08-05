/* Copyright (c) 2023-2025 hors<horsicq@gmail.com>
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
#ifndef DIALOGBITS8_H
#define DIALOGBITS8_H

#include <QDialog>
#include <QToolButton>
#include <QGroupBox>
#include <QRegExp>
#include "xlineeditvalidator.h"

namespace Ui {
class DialogBits8;
}

class DialogBits8 : public QDialog {
    Q_OBJECT

public:
    enum BitMode {
        BIT_MODE_8 = 8,
        BIT_MODE_16 = 16,
        BIT_MODE_32 = 32,
        BIT_MODE_64 = 64
    };

    explicit DialogBits8(QWidget *pParent = nullptr, bool bStayOnTop = false);
    ~DialogBits8();

    void setReadonly(bool bIsReadonly);
    void setValue(quint64 nValue, BitMode mode);
    void setValue_uint8(quint8 nValue) { setValue(nValue, BIT_MODE_8); }
    void setValue_uint16(quint16 nValue) { setValue(nValue, BIT_MODE_16); }
    void setValue_uint32(quint32 nValue) { setValue(nValue, BIT_MODE_32); }
    void setValue_uint64(quint64 nValue) { setValue(nValue, BIT_MODE_64); }
    
    quint64 getValue() const { return m_nValue; }
    quint8 getValue_uint8() const { return static_cast<quint8>(m_nValue); }
    quint16 getValue_uint16() const { return static_cast<quint16>(m_nValue); }
    quint32 getValue_uint32() const { return static_cast<quint32>(m_nValue); }
    quint64 getValue_uint64() const { return m_nValue; }

private slots:
    void on_lineEditHex_textChanged(const QString &sString);
    void on_lineEditSigned_textChanged(const QString &sString);
    void on_lineEditUnsigned_textChanged(const QString &sString);
    void on_lineEditBin_textChanged(const QString &sString);
    void onBitButtonToggled(bool bState);
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();

private:
    void setupValidators();
    void setupBitButtons();
    void configureForBitMode(BitMode mode);
    void updateDisplays();
    void updateBitButtons();
    void updateTextFields();
    void setControlsEnabled(bool bEnabled);
    QToolButton* getBitButton(int bitIndex) const;
    
    static const int MAX_BITS = 64;
    static const int BITS_PER_GROUP = 8;

    Ui::DialogBits8 *ui;
    quint64 m_nValue;
    BitMode m_currentMode;
    bool m_bIsReadonly;
    bool m_bUpdating;
    
    XLineEditValidator m_validatorHex;
    XLineEditValidator m_validatorSigned;
    XLineEditValidator m_validatorUnsigned;
    XLineEditValidator m_validatorBin;
    
    QList<QToolButton *> m_bitButtons;
};

#endif  // DIALOGBITS8_H
