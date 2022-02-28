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
#include "xlineedithex.h"

XLineEditHEX::XLineEditHEX(QWidget *pParent): QLineEdit(pParent)
{
    g_bIsColon=false;
    g_options={};

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

void XLineEditHEX::setOptions(XLineEditHEX::OPTIONS options)
{
    g_options=options;
}

void XLineEditHEX::setValue(quint8 nValue,HEXValidator::MODE validatorMode)
{
    g_validator.setData(validatorMode,0xFF);

    QString sText;

    if(validatorMode==HEXValidator::MODE_HEX)
    {
        sText=QString("%1").arg(nValue,2,16,QChar('0'));
    }
    else if(validatorMode==HEXValidator::MODE_DEC)
    {
         sText=QString("%1").arg(nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint8 nValue,HEXValidator::MODE validatorMode)
{
    setValue((quint8)nValue,validatorMode);
}

void XLineEditHEX::setValue(quint16 nValue,HEXValidator::MODE validatorMode)
{
    g_validator.setData(validatorMode,0xFFFF);

    QString sText;

    if(validatorMode==HEXValidator::MODE_HEX)
    {
        sText=QString("%1").arg(nValue,4,16,QChar('0'));
    }
    else if(validatorMode==HEXValidator::MODE_DEC)
    {
         sText=QString("%1").arg(nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint16 nValue,HEXValidator::MODE validatorMode)
{
    setValue((quint16)nValue,validatorMode);
}

void XLineEditHEX::setValue(quint32 nValue,HEXValidator::MODE validatorMode)
{
    g_validator.setData(validatorMode,0xFFFFFFFF);

    QString sText;

    if(validatorMode==HEXValidator::MODE_HEX)
    {
        if(g_bIsColon)
        {
            quint16 nHigh=(quint16)(nValue>>16);
            quint16 nLow=(quint16)(nValue);
            QString sHigh=QString("%1").arg(nHigh,4,16,QChar('0'));
            QString sLow=QString("%1").arg(nLow,4,16,QChar('0'));
            sText=QString("%1:%2").arg(sHigh,sLow);
        }
        else
        {
            sText=QString("%1").arg(nValue,8,16,QChar('0'));
        }
    }
    else if(validatorMode==HEXValidator::MODE_DEC)
    {
        sText=QString("%1").arg(nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint32 nValue,HEXValidator::MODE validatorMode)
{
    setValue((quint32)nValue,validatorMode);
}

void XLineEditHEX::setValue(quint64 nValue,HEXValidator::MODE validatorMode)
{
    g_validator.setData(validatorMode,0xFFFFFFFFFFFFFFFF);

    QString sText;

    if(validatorMode==HEXValidator::MODE_HEX)
    {
        if(g_bIsColon)
        {
            quint32 nHigh=(quint32)(nValue>>32);
            quint32 nLow=(quint32)(nValue);
            QString sHigh=QString("%1").arg(nHigh,8,16,QChar('0'));
            QString sLow=QString("%1").arg(nLow,8,16,QChar('0'));
            sText=QString("%1:%2").arg(sHigh,sLow);
        }
        else
        {
            sText=QString("%1").arg(nValue,16,16,QChar('0'));
        }
    }
    else if(validatorMode==HEXValidator::MODE_DEC)
    {
        sText=QString("%1").arg(nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint64 nValue,HEXValidator::MODE validatorMode)
{
    setValue((quint64)nValue,validatorMode);
}

void XLineEditHEX::setValueOS(quint64 nValue,HEXValidator::MODE validatorMode)
{
    MODE mode=MODE_32;

    if(sizeof(void *)==8)
    {
        mode=MODE_64;
    }

    setModeValue(mode,nValue,validatorMode);
}

void XLineEditHEX::setValue32_64(quint64 nValue, HEXValidator::MODE validatorMode)
{
    if(nValue>=0xFFFFFFFF)
    {
        setValue((quint64)nValue,validatorMode);
    }
    else
    {
        setValue((quint32)nValue,validatorMode);
    }
}

void XLineEditHEX::setModeValue(XLineEditHEX::MODE mode, quint64 nValue, HEXValidator::MODE validatorMode)
{
    if(mode==MODE_8)
    {
        setValue((quint8)nValue,validatorMode);
    }
    else if(mode==MODE_16)
    {
        setValue((quint16)nValue,validatorMode);
    }
    else if(mode==MODE_32)
    {
        setValue((quint32)nValue,validatorMode);
    }
    else if(mode==MODE_64)
    {
        setValue((quint64)nValue,validatorMode);
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

void XLineEditHEX::setUUID(QString sText)
{
    g_validator.setData(HEXValidator::MODE_UUID,0);

    setInputMask(">NNNNNNNN-NNNN-NNNN-NNNN-NNNNNNNNNNNN;0"); // TODO Check

    setText(sText);
}

quint64 XLineEditHEX::getValue()
{
    quint64 nResult=0;

    QString sText=text();

    if(g_validator.getMode()==HEXValidator::MODE_HEX)
    {
        if(g_bIsColon)
        {
            if(sText.contains(":"))
            {
                sText=sText.remove(":");
            }
        }

        nResult=sText.toULongLong(nullptr,16);
    }
    else if(g_validator.getMode()==HEXValidator::MODE_DEC)
    {
        nResult=sText.toULongLong(nullptr,10);
    }

    return nResult;
}

QVariant XLineEditHEX::_getValue()
{
    return vValue;
}

void XLineEditHEX::setText(QString sText)
{
    _setText(sText);
}

XLineEditHEX::MODE XLineEditHEX::getModeFromValue(quint64 nValue)
{
    MODE result=MODE_64;

    if(((quint64)nValue)>=0xFFFFFFFF)
    {
        result=MODE_64;
    }
    else if(((quint64)nValue)>=0xFFFF)
    {
        result=MODE_32;
    }
    else if(((quint64)nValue)>=0xFF)
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

    qint32 nMod=8;

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
        nMod=16; // TODO Check 14 symbols width?
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
    qint32 nSymbolWidth=getSymbolWidth(pWidget);
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

qint32 XLineEditHEX::getSymbolWidth(QWidget *pWidget)
{
    QFontMetrics fm(pWidget->font());

    return fm.boundingRect("W").width(); // TODO Check
}

void XLineEditHEX::setColon(bool bIsColon)
{
    g_bIsColon=bIsColon;
}

//void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)
//{
//    qDebug("void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)");
//}

void XLineEditHEX::_setText(QString sText)
{
    // TODO fix strings like 00011111111
    if(g_validator.getMode()==HEXValidator::MODE_TEXT)
    {
        emit valueChanged(text());
    }
    else if(g_validator.getMode()==HEXValidator::MODE_UUID)
    {
        emit valueChanged(displayText());
    }
    else if(g_validator.getMode()==HEXValidator::MODE_HEX)
    {
        quint64 nCurrentValue=sText.toULongLong(nullptr,16);

        if(vValue.toULongLong()!=nCurrentValue)
        {
            vValue=nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    }
    else if(g_validator.getMode()==HEXValidator::MODE_DEC)
    {
        quint64 nCurrentValue=sText.toULongLong(nullptr,10);

        if(vValue.toULongLong()!=nCurrentValue)
        {
            vValue=nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    }

    QLineEdit::setText(sText);
}

void XLineEditHEX::customContextMenu(const QPoint &nPos)
{
    QMenu contextMenu(this);

    QAction actionCopy(QString("%1: \"%2\"").arg(tr("Copy"),text()),this);
    connect(&actionCopy,SIGNAL(triggered()),this,SLOT(_copy()));
    contextMenu.addAction(&actionCopy);

    QAction actionCopyValue(QString("%1: \"%2\"").arg(tr("Copy"),QString::number(getValue())),this);
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

    _font.setBold(vValue.toULongLong()!=0); // TODO another modes

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
