// copyright (c) 2017-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef XLINEEDITHEX_H
#define XLINEEDITHEX_H

#include <QLineEdit>
#include <QVariant>
#include <QMenu>
#include <QAction>
#include <QShortcut>
#include <QClipboard>
#include <QApplication>
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
    };

    struct OPTIONS // TODO Check
    {
        bool bDemangle;
        bool bShowHexAddress;
        bool bShowHexOffset;
        bool bShowHexRelAddress;
        bool bShowDisasmAddress;
        bool bShowDisasmOffset;
        bool bShowDisasmRelAddress;
    };

    explicit XLineEditHEX(QWidget *pParent=nullptr);
    void setOptions(OPTIONS options);
    void setValue(quint8 nValue);
    void setValue(qint8 nValue);
    void setValue(quint16 nValue);
    void setValue(qint16 nValue);
    void setValue(quint32 nValue);
    void setValue(qint32 nValue);
    void setValue(quint64 nValue);
    void setValue(qint64 nValue);
    void setValue32_64(quint64 nValue);
    void setModeValue(MODE mode,quint64 nValue);
    void setStringValue(QString sText,qint32 nMaxLength=0);
    void setUUID(QString sText);
    quint64 getValue();
    void setText(QString sText);
    static MODE getModeFromValue(qint32 nValue);
    static QString getFormatString(MODE mode,qint64 nValue);
    static qint32 getWidthFromMode(QWidget *pWidget,MODE mode);
    static int getSymbolWidth(QWidget *pWidget);

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
    quint64 g_nValue;
    HEXValidator g_validator;
    OPTIONS g_options;
};

#define PXLineEditHEX XLineEditHEX *

#endif // XLINEEDITHEX_H
