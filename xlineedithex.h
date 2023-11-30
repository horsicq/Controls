/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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
#ifndef XLINEEDITHEX_H
#define XLINEEDITHEX_H

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QLineEdit>
#include <QMenu>
#include <QShortcut>
#include <QVariant>

#include "xlineeditvalidator.h"

// TODO setMode
class XLineEditHEX : public QLineEdit {
    Q_OBJECT

public:
    struct OPTIONS {
        bool bDemangle;              // TODO Check
        bool bShowHexAddress;        // TODO Check
        bool bShowHexOffset;         // TODO Check
        bool bShowHexRelAddress;     // TODO Check
        bool bShowDisasmAddress;     // TODO Check
        bool bShowDisasmOffset;      // TODO Check
        bool bShowDisasmRelAddress;  // TODO Check
    };

    enum _MODE {
        _MODE_TEXT = 0,
        _MODE_UUID,
        _MODE_SIZE, // TODO
        _MODE_HEX,
        _MODE_DEC,
        _MODE_SIGN_DEC
        // TODO more
    };

    explicit XLineEditHEX(QWidget *pParent = nullptr);

    void setOptions(const OPTIONS &options);

    void setValue(quint8 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(qint8 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(quint16 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(qint16 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(quint32 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(qint32 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(quint64 nValue, _MODE mode = _MODE_HEX); // TODO remove
    void setValue(qint64 nValue, _MODE mode = _MODE_HEX); // TODO remove

    void setValue_uint8(quint8 nValue, _MODE mode = _MODE_HEX);
    void setValue_int8(qint8 nValue, _MODE mode = _MODE_HEX);
    void setValue_uint16(quint16 nValue, _MODE mode = _MODE_HEX);
    void setValue_int16(qint16 nValue, _MODE mode = _MODE_HEX);
    void setValue_uint32(quint32 nValue, _MODE mode = _MODE_HEX);
    void setValue_int32(qint32 nValue, _MODE mode = _MODE_HEX);
    void setValue_uint64(quint64 nValue, _MODE mode = _MODE_HEX);
    void setValue_int64(qint64 nValue, _MODE mode = _MODE_HEX);
    void setValidatorModeValue(XLineEditValidator::MODE mode, QVariant varValue);
    void setMode(_MODE mode);
    //    void setValueOS(quint64 nValue, HEXValidator::MODE validatorMode = HEXValidator::MODE_HEX);
    void setValue32_64(quint64 nValue, _MODE mode = _MODE_HEX);
    void setStringValue(const QString &sText, qint32 nMaxLength = 0);
    void setUUID(const QString &sText);
    quint64 getIntValue();
    qint64 getSignValue();
    QVariant _getValue();
    void setText(const QString &sText);
    //    static MODE getModeFromValue(quint64 nValue);
    static QString getFormatString(XLineEditValidator::MODE mode, qint64 nValue);
    static qint32 getWidthFromMode(QWidget *pWidget, XLineEditValidator::MODE mode);
    static qint32 getSymbolWidth(QWidget *pWidget);
    void setColon(bool bIsColon);
    bool isFocused();
    void setValidatorMode(XLineEditValidator::MODE validatorMode);
    XLineEditValidator::MODE getValidatorMode();

protected:
    //    void keyPressEvent(QKeyEvent* keyEvent);
    bool eventFilter(QObject *pObject, QEvent *pEvent);

private slots:
    void _setText(const QString &sText);
    void customContextMenu(const QPoint &nPos);
    void updateFont();
    void _copy();
    void _copyValue();
    void _copySignValue();
    void _clearValue();

signals:
    void valueChanged(QVariant varValue);
    void focused(bool bState);

private:
    QVariant g_vValue;
    XLineEditValidator g_validator;
    OPTIONS g_options;
    bool g_bIsColon;
    bool g_bIsFocused;
    _MODE g_mode;
};

#define PXLineEditHEX XLineEditHEX *

#endif  // XLINEEDITHEX_H
