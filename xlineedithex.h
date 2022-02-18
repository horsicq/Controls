/* Copyright (c) 2017-2022 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
#include "hexvalidator.h"

class XLineEditHEX : public QLineEdit
{
    Q_OBJECT

public:
    enum MODE
    {
        MODE_8,
        MODE_16,
        MODE_32,
        MODE_64
        // TODO 128
    };

    struct OPTIONS
    {
        bool bDemangle;             // TODO Check
        bool bShowHexAddress;       // TODO Check
        bool bShowHexOffset;        // TODO Check
        bool bShowHexRelAddress;    // TODO Check
        bool bShowDisasmAddress;    // TODO Check
        bool bShowDisasmOffset;     // TODO Check
        bool bShowDisasmRelAddress; // TODO Check
    };

    explicit XLineEditHEX(QWidget *pParent=nullptr);
    void setOptions(OPTIONS options);
    void setValue(quint8 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(qint8 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(quint16 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(qint16 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(quint32 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(qint32 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(quint64 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue(qint64 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValueOS(quint64 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setValue32_64(quint64 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setModeValue(MODE mode,quint64 nValue,HEXValidator::MODE validatorMode=HEXValidator::MODE_HEX);
    void setStringValue(QString sText,qint32 nMaxLength=0);
    void setUUID(QString sText);
    quint64 getValue();
    QVariant _getValue();
    void setText(QString sText);
    static MODE getModeFromValue(quint64 nValue);
    static QString getFormatString(MODE mode,qint64 nValue);
    static qint32 getWidthFromMode(QWidget *pWidget,MODE mode);
    static qint32 getSymbolWidth(QWidget *pWidget);
    void setColon(bool bIsColon);

protected:
//    void keyPressEvent(QKeyEvent* keyEvent);

private slots:
    void _setText(QString sText);
    void customContextMenu(const QPoint &nPos);
    void updateFont();
    void _copy();
    void _copyValue();

signals:
    void valueChanged(quint64 nValue);
    void valueChanged(QString sValue);

private:
    QVariant vValue;
    HEXValidator g_validator;
    OPTIONS g_options;
    bool g_bIsColon;
};

#define PXLineEditHEX XLineEditHEX *

#endif // XLINEEDITHEX_H
