// copyright (c) 2017-2019 hors<horsicq@gmail.com>
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
#include "qlineedithex.h"

QLineEditHEX::QLineEditHEX(QWidget *parent): QLineEdit(parent)
{
    nValue=0;
    //    setReadOnly(true); // TODO
    setAlignment(Qt::AlignHCenter);
    setInputMask("HHHHHHHHHHHHHHHH");

    connect(this,SIGNAL(textChanged(QString)),this,SLOT(_setText(QString)));

//    setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
}

void QLineEditHEX::setValue(quint8 value)
{
    setInputMask("HH");
    QString sText=QString("%1").arg(value,2,16,QChar('0'));
    setText(sText);
}

void QLineEditHEX::setValue(qint8 value)
{
    setValue((quint8)value);
}

void QLineEditHEX::setValue(quint16 value)
{
    setInputMask("HHHH");
    QString sText=QString("%1").arg(value,4,16,QChar('0'));
    setText(sText);
}

void QLineEditHEX::setValue(qint16 value)
{
    setValue((quint16)value);
}

void QLineEditHEX::setValue(quint32 value)
{
    setInputMask("HHHHHHHH");
    QString sText=QString("%1").arg(value,8,16,QChar('0'));
    setText(sText);
}

void QLineEditHEX::setValue(qint32 value)
{
    setValue((quint32)value);
}

void QLineEditHEX::setValue(quint64 value)
{
    setInputMask("HHHHHHHHHHHHHHHH");
    QString sText=QString("%1").arg(value,16,16,QChar('0'));
    setText(sText);
}

void QLineEditHEX::setValue(qint64 value)
{
    setValue((quint64)value);
}

void QLineEditHEX::setValue32_64(quint64 value)
{
    if(value>=0xFFFFFFFF)
    {
        setValue((quint64)value);
    }
    else
    {
        setValue((quint32)value);
    }
}

void QLineEditHEX::setStringValue(QString sText, qint32 nMaxLength)
{
    setInputMask("");
    if(nMaxLength)
    {
        setMaxLength(nMaxLength);
    }
    setText(sText);
}

quint64 QLineEditHEX::getValue()
{
    quint64 nResult=0;

    nResult=text().toULongLong(0,16);

    return nResult;
}

void QLineEditHEX::_setText(QString sText)
{
    quint64 nCurrentValue=sText.toULongLong(0,16);

    if(nValue!=nCurrentValue)
    {
        nValue=nCurrentValue;

        emit valueChanged(nCurrentValue);
    }
}

void QLineEditHEX::customContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);

    contextMenu.exec(mapToGlobal(pos));
}
