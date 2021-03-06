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
#include "xlineedithex.h"

XLineEditHEX::XLineEditHEX(QWidget *pParent): QLineEdit(pParent)
{
    g_nValue=0;
    updateFont();

    setAlignment(Qt::AlignHCenter);

    connect(this,SIGNAL(textChanged(QString)),this,SLOT(_setText(QString)));
    setValidator(&g_validator);

    // TODO Context Menu
    // TODO Copy
    // TODO clear
    // mb TODO 10/16
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
}

void XLineEditHEX::setValue(quint8 nValue)
{
    g_validator.setData(HEXValidator::MODE_HEX,0xFF);
    QString sText=QString("%1").arg(nValue,2,16,QChar('0'));
    setText(sText);
}

void XLineEditHEX::setValue(qint8 nValue)
{
    setValue((quint8)nValue);
}

void XLineEditHEX::setValue(quint16 nValue)
{
    g_validator.setData(HEXValidator::MODE_HEX,0xFFFF);
    QString sText=QString("%1").arg(nValue,4,16,QChar('0'));
    setText(sText);
}

void XLineEditHEX::setValue(qint16 nValue)
{
    setValue((quint16)nValue);
}

void XLineEditHEX::setValue(quint32 nValue)
{
    g_validator.setData(HEXValidator::MODE_HEX,0xFFFFFFFF);
    QString sText=QString("%1").arg(nValue,8,16,QChar('0'));
    setText(sText);
}

void XLineEditHEX::setValue(qint32 nValue)
{
    setValue((quint32)nValue);
}

void XLineEditHEX::setValue(quint64 nValue)
{
    g_validator.setData(HEXValidator::MODE_HEX,0xFFFFFFFFFFFFFFFF);
    QString sText=QString("%1").arg(nValue,16,16,QChar('0'));
    setText(sText);
}

void XLineEditHEX::setValue(qint64 nValue)
{
    setValue((quint64)nValue);
}

void XLineEditHEX::setValue32_64(quint64 nValue)
{
    if(nValue>=0xFFFFFFFF)
    {
        setValue((quint64)nValue);
    }
    else
    {
        setValue((quint32)nValue);
    }
}

void XLineEditHEX::setModeValue(XLineEditHEX::MODE mode, quint64 nValue)
{
    if(mode==MODE_8)
    {
        setValue((quint8)nValue);
    }
    else if(mode==MODE_16)
    {
        setValue((quint16)nValue);
    }
    else if(mode==MODE_32)
    {
        setValue((quint32)nValue);
    }
    else if(mode==MODE_64)
    {
        setValue((quint64)nValue);
    }
}

void XLineEditHEX::setStringValue(QString sText, qint32 nMaxLength)
{
    g_validator.setData(HEXValidator::MODE_TEXT,0);

    if(nMaxLength)
    {
        setMaxLength(nMaxLength);
    }

    setText(sText);
}

quint64 XLineEditHEX::getValue()
{
    quint64 nResult=0;

    nResult=text().toULongLong(nullptr,16);

    return nResult;
}

void XLineEditHEX::setText(QString sText)
{
    QLineEdit::setText(sText);
    _setText(sText);
}

XLineEditHEX::MODE XLineEditHEX::getModeFromSize(qint32 nSize)
{
    MODE result=MODE_64;

    if(((quint64)nSize)>=0xFFFFFFFF)
    {
        result=MODE_64;
    }
    else if(((quint64)nSize)>=0xFFFF)
    {
        result=MODE_32;
    }
    else if(((quint64)nSize)>=0xFF)
    {
        result=MODE_16;
    }
    else
    {
        result=MODE_8;
    }

    return result;
}

QString XLineEditHEX::getFormatString(XLineEditHEX::MODE mode, qint64 nValue)
{
    QString sResult;

    int nMod=8;

    if(mode==MODE_8)
    {
        nMod=2;
    }
    else if(mode==MODE_16)
    {
        nMod=4;
    }
    else if(mode==MODE_32)
    {
        nMod=8;
    }
    else if(mode==MODE_64)
    {
        nMod=16;
    }

    sResult=QString("%1").arg(nValue,nMod,16,QChar('0'));

    if(nValue<0)
    {
        sResult=sResult.right(nMod);
    }

    return sResult;
}

qint32 XLineEditHEX::getWidthFromMode(QWidget *pWidget,XLineEditHEX::MODE mode)
{
    int nSymbolWidth=getSymbolWidth(pWidget);
    qint32 nResult=nSymbolWidth*8;

    if(mode==MODE_8)
    {
        nResult=nSymbolWidth*4;
    }
    else if(mode==MODE_16)
    {
        nResult=nSymbolWidth*6;
    }
    else if(mode==MODE_32)
    {
        nResult=nSymbolWidth*8;
    }
    else if(mode==MODE_64)
    {
        nResult=nSymbolWidth*12;
    }

    return nResult;
}

int XLineEditHEX::getSymbolWidth(QWidget *pWidget)
{
    QFontMetrics fm(pWidget->font());

    return fm.boundingRect("W").width(); // TODO Check
}

//void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)
//{
//    qDebug("void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)");
//}

void XLineEditHEX::_setText(QString sText)
{
    if(g_validator.getMode()!=HEXValidator::MODE_TEXT)
    {
        quint64 nCurrentValue=sText.toULongLong(nullptr,16);

        if(g_nValue!=nCurrentValue)
        {
            g_nValue=nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    }
}

void XLineEditHEX::customContextMenu(const QPoint &nPos)
{
    QMenu contextMenu(this);

    QAction actionCopy(QString("%1: \"%2\"").arg(tr("Copy")).arg(text()),this);
    connect(&actionCopy,SIGNAL(triggered()),this,SLOT(_copy()));
    contextMenu.addAction(&actionCopy);

    QAction actionCopyValue(QString("%1: \"%2\"").arg(tr("Copy")).arg(QString::number(getValue())),this);
    connect(&actionCopyValue,SIGNAL(triggered()),this,SLOT(_copyValue()));

    if(g_validator.getMode()!=HEXValidator::MODE_TEXT)
    {
        contextMenu.addAction(&actionCopyValue);
    }

    contextMenu.exec(mapToGlobal(nPos));
}

void XLineEditHEX::updateFont()
{
    QFont _font=font();

    _font.setBold(g_nValue!=0);

    setFont(_font);
}

void XLineEditHEX::_copy()
{
    QApplication::clipboard()->setText(text());
}

void XLineEditHEX::_copyValue()
{
    QApplication::clipboard()->setText(QString::number(getValue()));
}
