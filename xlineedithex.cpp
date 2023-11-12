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
#include "xlineedithex.h"

XLineEditHEX::XLineEditHEX(QWidget *pParent) : QLineEdit(pParent)
{
    g_bIsColon = false;
    g_bIsFocused = false;
    g_options = {};
    g_mode = _MODE_HEX;

    updateFont();

    setAlignment(Qt::AlignHCenter);

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(_setText(QString)));
    setValidator(&g_validator);

    // TODO Context Menu
    // TODO Copy
    // TODO clear
    // mb TODO 10/16
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    installEventFilter(this);
}

void XLineEditHEX::setOptions(const OPTIONS &options)
{
    g_options = options;
}

void XLineEditHEX::setValue(quint8 nValue, _MODE mode)
{
    g_mode = mode;

    QString sText;

    if (mode == _MODE_HEX) {
        g_validator.setMode(XLineEditValidator::MODE_HEX_8);
        sText = QString("%1").arg(nValue, 2, 16, QChar('0'));
    } else if (mode == _MODE_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_DEC_8);
        sText = QString("%1").arg(nValue);
    } else if (mode == _MODE_SIGN_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_8);
        sText = QString("%1").arg((qint8)nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint8 nValue, _MODE mode)
{
    setValue((quint8)nValue, mode);
}

void XLineEditHEX::setValue(quint16 nValue, _MODE mode)
{
    g_mode = mode;

    QString sText;

    if (mode == _MODE_HEX) {
        g_validator.setMode(XLineEditValidator::MODE_HEX_16);
        sText = QString("%1").arg(nValue, 4, 16, QChar('0'));
    } else if (mode == _MODE_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_DEC_16);
        sText = QString("%1").arg(nValue);
    } else if (mode == _MODE_SIGN_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_16);
        sText = QString("%1").arg((qint16)nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint16 nValue, _MODE mode)
{
    setValue((quint16)nValue, mode);
}

void XLineEditHEX::setValue(quint32 nValue, _MODE mode)
{
    g_mode = mode;

    QString sText;

    if (mode == _MODE_HEX) {
        g_validator.setMode(XLineEditValidator::MODE_HEX_32);
        if (g_bIsColon) {
            quint16 nHigh = (quint16)(nValue >> 16);
            quint16 nLow = (quint16)(nValue);
            QString sHigh = QString("%1").arg(nHigh, 4, 16, QChar('0'));
            QString sLow = QString("%1").arg(nLow, 4, 16, QChar('0'));
            sText = QString("%1:%2").arg(sHigh, sLow);
        } else {
            sText = QString("%1").arg(nValue, 8, 16, QChar('0'));
        }
    } else if (mode == _MODE_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_DEC_32);
        sText = QString("%1").arg(nValue);
    } else if (mode == _MODE_SIGN_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_32);
        sText = QString("%1").arg((qint32)nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint32 nValue, _MODE mode)
{
    setValue((quint32)nValue, mode);
}

void XLineEditHEX::setValue(quint64 nValue, _MODE mode)
{
    g_mode = mode;

    QString sText;

    if (mode == _MODE_HEX) {
        g_validator.setMode(XLineEditValidator::MODE_HEX_64);
        if (g_bIsColon) {
            quint32 nHigh = (quint32)(nValue >> 32);
            quint32 nLow = (quint32)(nValue);
            QString sHigh = QString("%1").arg(nHigh, 8, 16, QChar('0'));
            QString sLow = QString("%1").arg(nLow, 8, 16, QChar('0'));
            sText = QString("%1:%2").arg(sHigh, sLow);
        } else {
            sText = QString("%1").arg(nValue, 16, 16, QChar('0'));
        }
    } else if (mode == _MODE_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_DEC_64);
        sText = QString("%1").arg(nValue);
    } else if (mode == _MODE_SIGN_DEC) {
        g_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_64);
        sText = QString("%1").arg((qint64)nValue);
    }

    setText(sText);
}

void XLineEditHEX::setValue(qint64 nValue, _MODE mode)
{
    setValue((quint64)nValue, mode);
}

void XLineEditHEX::setValidatorModeValue(XLineEditValidator::MODE mode, QVariant varValue)
{
    if (mode == XLineEditValidator::MODE_HEX_8) {
        setValue((quint8)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_HEX_16) {
        setValue((quint16)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_HEX_32) {
        setValue((quint32)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_HEX_64) {
        setValue((quint64)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_DEC_8) {
        setValue((quint8)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_DEC_16) {
        setValue((quint16)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_DEC_32) {
        setValue((quint32)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_DEC_64) {
        setValue((quint64)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_8) {
        setValue((quint8)varValue.toULongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_16) {
        setValue((quint16)varValue.toULongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_32) {
        setValue((quint32)varValue.toULongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_64) {
        setValue((quint64)varValue.toULongLong(), _MODE_SIGN_DEC);
    }
}

void XLineEditHEX::setMode(_MODE mode)
{
    XLineEditValidator::MODE validatorMode = getValidatorMode();

    if (mode == _MODE_HEX) {
        if ((validatorMode == XLineEditValidator::MODE_HEX_8) || (validatorMode == XLineEditValidator::MODE_DEC_8) ||
            (validatorMode == XLineEditValidator::MODE_SIGN_DEC_8)) {
            validatorMode = XLineEditValidator::MODE_HEX_8;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_16) || (validatorMode == XLineEditValidator::MODE_DEC_16) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_16)) {
            validatorMode = XLineEditValidator::MODE_HEX_16;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_32) || (validatorMode == XLineEditValidator::MODE_DEC_32) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_32)) {
            validatorMode = XLineEditValidator::MODE_HEX_32;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_64) || (validatorMode == XLineEditValidator::MODE_DEC_64) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_64)) {
            validatorMode = XLineEditValidator::MODE_HEX_64;
        }
    } else if (mode == _MODE_DEC) {
        if ((validatorMode == XLineEditValidator::MODE_HEX_8) || (validatorMode == XLineEditValidator::MODE_DEC_8) ||
            (validatorMode == XLineEditValidator::MODE_SIGN_DEC_8)) {
            validatorMode = XLineEditValidator::MODE_DEC_8;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_16) || (validatorMode == XLineEditValidator::MODE_DEC_16) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_16)) {
            validatorMode = XLineEditValidator::MODE_DEC_16;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_32) || (validatorMode == XLineEditValidator::MODE_DEC_32) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_32)) {
            validatorMode = XLineEditValidator::MODE_DEC_32;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_64) || (validatorMode == XLineEditValidator::MODE_DEC_64) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_64)) {
            validatorMode = XLineEditValidator::MODE_DEC_64;
        }
    } else if (mode == _MODE_SIGN_DEC) {
        if ((validatorMode == XLineEditValidator::MODE_HEX_8) || (validatorMode == XLineEditValidator::MODE_DEC_8) ||
            (validatorMode == XLineEditValidator::MODE_SIGN_DEC_8)) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_8;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_16) || (validatorMode == XLineEditValidator::MODE_DEC_16) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_16)) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_16;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_32) || (validatorMode == XLineEditValidator::MODE_DEC_32) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_32)) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_32;
        } else if ((validatorMode == XLineEditValidator::MODE_HEX_64) || (validatorMode == XLineEditValidator::MODE_DEC_64) ||
                   (validatorMode == XLineEditValidator::MODE_SIGN_DEC_64)) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_64;
        }
    }

    QVariant varValue = _getValue();

    setValidatorModeValue(validatorMode, varValue);
}

// void XLineEditHEX::setValueOS(quint64 nValue, HEXValidator::MODE validatorMode)
//{
//     MODE mode = MODE_32;

//    if (sizeof(void *) == 8) {
//        mode = MODE_64;
//    }

//    setModeValue(mode, nValue, validatorMode);
//}

void XLineEditHEX::setValue32_64(quint64 nValue, _MODE mode)
{
    if (nValue >= 0xFFFFFFFF) {
        setValue((quint64)nValue, mode);
    } else {
        setValue((quint32)nValue, mode);
    }
}

void XLineEditHEX::setStringValue(const QString &sText, qint32 nMaxLength)
{
    g_mode = _MODE_TEXT;
    g_validator.setMode(XLineEditValidator::MODE_TEXT);

    if (nMaxLength) {
        setMaxLength(nMaxLength);
    }

    setText(sText);
}

void XLineEditHEX::setUUID(const QString &sText)
{
    g_mode = _MODE_UUID;
    g_validator.setMode(XLineEditValidator::MODE_UUID);

    setInputMask(">NNNNNNNN-NNNN-NNNN-NNNN-NNNNNNNNNNNN;0");  // TODO Check

    setText(sText);
}

quint64 XLineEditHEX::getIntValue()
{
    quint64 nResult = 0;

    QString sText = text();

    if (g_mode == _MODE_HEX) {
        if (g_bIsColon) {
            if (sText.contains(":")) {
                sText = sText.remove(":");
            }
        }

        nResult = sText.toULongLong(nullptr, 16);
    } else if (g_mode == _MODE_DEC) {
        nResult = sText.toULongLong(nullptr, 10);
    } else if (g_mode == _MODE_SIGN_DEC) {
        nResult = sText.toLongLong(nullptr, 10);
    }

    return nResult;
}

qint64 XLineEditHEX::getSignValue()
{
    qint64 nResult = 0;

    QString sText = text();

    if (g_mode == _MODE_HEX) {
        if (g_bIsColon) {
            if (sText.contains(":")) {
                sText = sText.remove(":");
            }
        }

        nResult = sText.toULongLong(nullptr, 16);
        // TODO
    } else if (g_mode == _MODE_DEC) {
        nResult = sText.toULongLong(nullptr, 10);
    } else if (g_mode == _MODE_SIGN_DEC) {
        nResult = sText.toLongLong(nullptr, 10);
    }

    return nResult;
}

QVariant XLineEditHEX::_getValue()
{
    return g_vValue;
}

void XLineEditHEX::setText(const QString &sText)
{
    _setText(sText);
}

// XLineEditHEX::MODE XLineEditHEX::getModeFromValue(quint64 nValue)
//{
//     MODE result = MODE_64;

//    if (((quint64)nValue) > 0xFFFFFFFF) {
//        result = MODE_64;
//    } else if (((quint64)nValue) > 0xFFFF) {
//        result = MODE_32;
//    } else if (((quint64)nValue) > 0xFF) {
//        result = MODE_16;
//    } else {
//        result = MODE_8;
//    }

//    return result;
//}

QString XLineEditHEX::getFormatString(XLineEditValidator::MODE mode, qint64 nValue)
{
    QString sResult;

    qint32 nMod = 8;

    if (mode == XLineEditValidator::MODE_HEX_8) {
        nMod = 2;
    } else if (mode == XLineEditValidator::MODE_HEX_16) {
        nMod = 4;
    } else if (mode == XLineEditValidator::MODE_HEX_32) {
        nMod = 8;
    } else if (mode == XLineEditValidator::MODE_HEX_64) {
        nMod = 16;  // TODO Check 14 symbols width?
    }

    sResult = QString("%1").arg(nValue, nMod, 16, QChar('0'));

    if (nValue < 0) {
        sResult = sResult.right(nMod);
    }

    return sResult;
}

qint32 XLineEditHEX::getWidthFromMode(QWidget *pWidget, XLineEditValidator::MODE mode)
{
    qint32 nSymbolWidth = getSymbolWidth(pWidget);
    qint32 nResult = nSymbolWidth * 8;

    // TODO Check
    if (mode == XLineEditValidator::MODE_HEX_8) {
        nResult = nSymbolWidth * 4;
    } else if (mode == XLineEditValidator::MODE_HEX_16) {
        nResult = nSymbolWidth * 6;
    } else if (mode == XLineEditValidator::MODE_HEX_32) {
        nResult = nSymbolWidth * 8;
    } else if (mode == XLineEditValidator::MODE_HEX_64) {
        nResult = nSymbolWidth * 12;
    }

    return nResult;
}

qint32 XLineEditHEX::getSymbolWidth(QWidget *pWidget)
{
    QFontMetrics fm(pWidget->font());

    return fm.boundingRect("W").width();  // TODO Check
}

void XLineEditHEX::setColon(bool bIsColon)
{
    g_bIsColon = bIsColon;
}

bool XLineEditHEX::isFocused()
{
    return g_bIsFocused;
}

void XLineEditHEX::setValidatorMode(XLineEditValidator::MODE validatorMode)
{
    g_validator.setMode(validatorMode);
    //  TODO
}

XLineEditValidator::MODE XLineEditHEX::getValidatorMode()
{
    return g_validator.getMode();
}

bool XLineEditHEX::eventFilter(QObject *pObject, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::FocusIn) {
        g_bIsFocused = true;

        emit focused(true);
    } else if (pEvent->type() == QEvent::FocusOut) {
        g_bIsFocused = false;

        emit focused(false);
    }

    return QLineEdit::eventFilter(pObject, pEvent);
}

// void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)
//{
//     qDebug("void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)");
// }

void XLineEditHEX::_setText(const QString &sText)
{
    // TODO fix strings like 00011111111
    if (g_mode == _MODE_TEXT) {
        emit valueChanged(text());
    } else if (g_mode == _MODE_UUID) {
        emit valueChanged(displayText());
    } else if (g_mode == _MODE_HEX) {
        quint64 nCurrentValue = sText.toULongLong(nullptr, 16);

        if (g_vValue.toULongLong() != nCurrentValue) {
            g_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    } else if (g_mode == _MODE_DEC) {
        quint64 nCurrentValue = sText.toULongLong(nullptr, 10);

        if (g_vValue.toULongLong() != nCurrentValue) {
            g_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    } else if (g_mode == _MODE_SIGN_DEC) {
        qint64 nCurrentValue = sText.toLongLong(nullptr, 10);
        qint64 _nValue = g_vValue.toLongLong();

        if (_nValue != nCurrentValue) {
            g_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    }

    QLineEdit::setText(sText);
}

void XLineEditHEX::customContextMenu(const QPoint &nPos)
{
    QMenu contextMenu(this);

    QAction actionCopy(QString("%1: \"%2\"").arg(tr("Copy"), text()), this);
    connect(&actionCopy, SIGNAL(triggered()), this, SLOT(_copy()));
    contextMenu.addAction(&actionCopy);

    QAction actionCopyValue(QString("%1: \"%2\"").arg(tr("Copy"), QString::number(getIntValue())), this);
    QAction actionCopySignValue(this);

    if ((g_mode == _MODE_HEX) || (g_mode == _MODE_DEC) || (g_mode == _MODE_SIGN_DEC)) {
        connect(&actionCopyValue, SIGNAL(triggered()), this, SLOT(_copyValue()));
        contextMenu.addAction(&actionCopyValue);

        if (getIntValue() != (quint64)getSignValue()) {
            actionCopySignValue.setText(QString("%1: \"%2\"").arg(tr("Copy"), QString::number(getSignValue())));
            connect(&actionCopySignValue, SIGNAL(triggered()), this, SLOT(_copySignValue()));
            contextMenu.addAction(&actionCopySignValue);
        }
    }

    QAction actionClearValue(tr("Clear"), this);

    if (!isReadOnly()) {
        contextMenu.addSeparator();

        connect(&actionClearValue, SIGNAL(triggered()), this, SLOT(_clearValue()));
        contextMenu.addAction(&actionClearValue);
    }

    contextMenu.exec(mapToGlobal(nPos));
}

void XLineEditHEX::updateFont()
{
    QFont _font = font();

    if (g_mode == _MODE_SIGN_DEC) {
        _font.setBold(g_vValue.toLongLong() != 0);
    } else {
        _font.setBold(g_vValue.toULongLong() != 0);
    }
    // TODO another modes

    setFont(_font);
}

void XLineEditHEX::_copy()
{
    QApplication::clipboard()->setText(text());
}

void XLineEditHEX::_copyValue()
{
    QApplication::clipboard()->setText(QString::number(getIntValue()));
}

void XLineEditHEX::_copySignValue()
{
    QApplication::clipboard()->setText(QString::number(getSignValue()));
}

void XLineEditHEX::_clearValue()
{
    XLineEditValidator::MODE mode = g_validator.getMode();

    // TODO UUID
    if (mode == XLineEditValidator::MODE_TEXT) {
        setStringValue("");
    } else {
        setValidatorModeValue(g_validator.getMode(), 0);
    }

    emit textEdited(text());
}
