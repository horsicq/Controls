/* Copyright (c) 2020-2024 hors<horsicq@gmail.com>
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
#include "xabstracttableview.h"

XAbstractTableView::XAbstractTableView(QWidget *pParent) : XShortcutstScrollArea(pParent)
{
    g_bIsActive = false;
    //    g_bIsBlinkingCursorEnable = false;
    g_bMouseResizeColumn = false;
    g_bMouseSelection = false;
    g_nViewPosStart = 0;
    g_nCharWidth = 0;
    g_nCharHeight = 0;
    g_nLinesProPage = 0;
    g_nLineHeight = 0;
    g_nTotalScrollCount = 0;
    g_nViewWidth = 0;
    g_nViewHeight = 0;
    g_nTableWidth = 0;
    g_nSelectionInitOffset = -1;
    g_nSelectionInitSize = 0;
    g_nNumberOfRows = 0;
    //    g_nCursorDelta = 0;
    g_nXViewPos = 0;
    g_nHeaderHeight = 20;
    g_nLineDelta = 0;
    g_nSideDelta = 0;
    g_state = {};
    g_bBlink = false;
    g_bLastColumnStretch = false;
    g_bHeaderVisible = false;
    g_bColumnFixed = false;
    g_bVerticalLinesVisible = false;
    g_bHorisontalLinesVisible = false;
    g_bIsMapEnable = false;
    g_nMapWidth = 0;
    g_nMapCount = 1;

    g_nResizeColumnNumber = 0;

    g_bHeaderClickButton = false;
    g_nHeaderClickColumnNumber = 0;

    g_nCurrentBlockViewPos = 0;
    g_nCurrentBlockViewSize = 0;

    g_bIsSelectionEnable = true;
    g_nMaxSelectionViewSize = 0;
    g_bIsContextMenuEnable = true;

    setContextMenuPolicy(Qt::CustomContextMenu);

    setHeaderVisible(true);
    setColumnFixed(false);
    setVerticalLinesVisible(true);
    setLineDelta(0);
    setSideDelta(3);
    // TODO Cursor off default !!! // TODO Check

    installEventFilter(this);  // mb TODO move to setActive
}

XAbstractTableView::~XAbstractTableView()
{
}

void XAbstractTableView::setActive(bool bIsActive)
{
    bool bChanged = (g_bIsActive != bIsActive);

    if (bChanged) {
        if (bIsActive) {
            connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_customContextMenu(QPoint)));
            connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScroll()));
            connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horisontalScroll()));
        } else {
            // TODO Check
            disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_customContextMenu(QPoint)));
            disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScroll()));
            disconnect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horisontalScroll()));

            verticalScrollBar()->setRange(0, 0);
            horizontalScrollBar()->setRange(0, 0);
        }

        //        if (g_bIsBlinkingCursorEnable) {
        //            setBlinkingCursor(bIsActive);
        //        }

        setMouseTracking(bIsActive);  // Important

        g_bIsActive = bIsActive;
    }
}

bool XAbstractTableView::isActive()
{
    return g_bIsActive;
}

void XAbstractTableView::clear()
{
#ifdef QT_DEBUG
    qDebug("void XAbstractTableView::clear()");
#endif
    setActive(false);
    // TODO
    update();  // Check viewport()->update();
}

void XAbstractTableView::addColumn(const QString &sTitle, qint32 nWidth, bool bClickable, bool bEnable)
{
    COLUMN column = {};

    column.bEnable = bEnable;
    column.nWidth = nWidth;
    column.sTitle = sTitle;
    column.bClickable = bClickable;

    g_listColumns.append(column);
}

void XAbstractTableView::setColumnTitle(qint32 nNumber, const QString &sTitle)
{
    if (nNumber < g_listColumns.count()) {
        g_listColumns[nNumber].sTitle = sTitle;
    }
}

void XAbstractTableView::setColumnEnabled(qint32 nNumber, bool bState)
{
    if (nNumber < g_listColumns.count()) {
        g_listColumns[nNumber].bEnable = bState;
    }
}

void XAbstractTableView::setColumnWidth(qint32 nNumber, qint32 nWidth)
{
    if (nNumber < g_listColumns.count()) {
        g_listColumns[nNumber].nWidth = nWidth;
    }
}

qint32 XAbstractTableView::getColumnWidth(qint32 nNumber)
{
    qint32 nResult = 0;

    if (nNumber < g_listColumns.count()) {
        nResult = g_listColumns[nNumber].nWidth;
    }

    return nResult;
}

void XAbstractTableView::paintEvent(QPaintEvent *pEvent)
{
#ifdef QT_DEBUG
//    qDebug("XAbstractTableView::paintEvent");
//    QElapsedTimer timer;
//    timer.start();
#endif
    QPainter *pPainter = new QPainter(this->viewport());
    pPainter->setFont(g_fontText);
    pPainter->setPen(viewport()->palette().color(QPalette::WindowText));
    pPainter->setBackgroundMode(Qt::TransparentMode);

    if (isActive()) {
        //        if (g_rectCursorSquare != pEvent->rect())
        {
            startPainting(pPainter);
            // TODO paint visible only

            qint32 nTopLeftY = pEvent->rect().topLeft().y();
            qint32 nTopLeftX = pEvent->rect().topLeft().x() - g_nXViewPos;
            qint32 nScreenWidth = pEvent->rect().width();
            qint32 nHeight = pEvent->rect().height();

            if (isMapEnable()) {
                nScreenWidth -= getMapWidth();
            }

            qint32 nNumberOfColumns = g_listColumns.count();

            if (nNumberOfColumns) {
                qint32 nX = nTopLeftX;

                qint32 nHeaderHeight = (g_bHeaderVisible) ? (g_nHeaderHeight) : (0);

                for (qint32 i = 0; i < nNumberOfColumns; i++) {
                    if (g_listColumns.at(i).bEnable) {
                        qint32 nColumnWidth = g_listColumns.at(i).nWidth;

                        pPainter->fillRect(nX, nTopLeftY + nHeaderHeight, nColumnWidth, nHeight - nHeaderHeight, viewport()->palette().color(QPalette::Base));

                        paintColumn(pPainter, i, nX, nTopLeftY + nHeaderHeight, nColumnWidth, nHeight - nHeaderHeight);

                        for (qint32 j = 0; j < g_nLinesProPage; j++) {
                            paintCell(pPainter, j, i, nX, nTopLeftY + nHeaderHeight + (j * g_nLineHeight), nColumnWidth, g_nLineHeight);
                        }

                        nX += nColumnWidth;
                    }
                }

                // Rest
                if (nScreenWidth - nX > 0) {
                    pPainter->fillRect(nX, nTopLeftY, nScreenWidth - nX, nHeight, viewport()->palette().color(QPalette::Base));
                }

                nX = nTopLeftX;

                // Draw lines and headers
                for (qint32 i = 0; i < nNumberOfColumns; i++) {
                    qint32 nColumnWidth = g_listColumns.at(i).nWidth;

                    if (g_listColumns.at(i).bEnable) {
                        if (nHeaderHeight > 0) {
                            QStyleOptionButton styleOptionButton;

                            if ((g_bHeaderClickButton) && (g_nHeaderClickColumnNumber == i)) {
                                styleOptionButton.state = QStyle::State_Raised;  // TODO Check
                            } else {
                                styleOptionButton.state = QStyle::State_Enabled;
                            }

                            styleOptionButton.rect = QRect(nX, nTopLeftY, nColumnWidth, nHeaderHeight);

                            g_pushButtonHeader.style()->drawControl(QStyle::CE_PushButton, &styleOptionButton, pPainter, &g_pushButtonHeader);

                            paintTitle(pPainter, i, nX, nTopLeftY, nColumnWidth, nHeaderHeight, g_listColumns.at(i).sTitle);
                        }

                        if (g_bVerticalLinesVisible) {
                            pPainter->drawLine(nX + nColumnWidth, nTopLeftY + nHeaderHeight, nX + nColumnWidth, nTopLeftY + nHeight);
                        }

                        if (g_bHorisontalLinesVisible) {
                            for (qint32 j = 0; j < g_nLinesProPage; j++) {
                                pPainter->drawLine(nX, nTopLeftY + nHeaderHeight + ((j + 1) * g_nLineHeight), nX + nColumnWidth,
                                                   nTopLeftY + nHeaderHeight + (j + 1) * g_nLineHeight);
                            }
                        }

                        nX += nColumnWidth;
                    }
                }

                if (isMapEnable()) {
                    qint32 nMapWidth = getMapWidth();
                    qint32 _nX = nScreenWidth;
                    pPainter->fillRect(_nX, nTopLeftY + nHeaderHeight, nMapWidth, nHeight - nHeaderHeight, viewport()->palette().color(QPalette::Base));

                    paintMap(pPainter, _nX, nTopLeftY + nHeaderHeight, nMapWidth, nHeight - nHeaderHeight);

                    if (nHeaderHeight > 0) {
                        QStyleOptionButton styleOptionButton;
                        styleOptionButton.state = QStyle::State_Enabled;
                        styleOptionButton.rect = QRect(_nX, nTopLeftY, nMapWidth, nHeaderHeight);

                        g_pushButtonHeader.style()->drawControl(QStyle::CE_PushButton, &styleOptionButton, pPainter, &g_pushButtonHeader);
                    }
                }
            }

            endPainting(pPainter);
        }

        //        // Draw cursor
        //        // TODO Cursor off
        //        if (g_rectCursorSquare.width() && g_rectCursorSquare.height()) {
        //            if (g_bBlink) {
        //                pPainter->setPen(viewport()->palette().color(QPalette::Highlight));
        //                pPainter->fillRect(g_rectCursorSquare, this->palette().color(QPalette::WindowText));
        //            } else {
        //                pPainter->setPen(viewport()->palette().color(QPalette::WindowText));
        //                pPainter->fillRect(g_rectCursorSquare, this->palette().color(QPalette::Base));
        //            }

        //            if (g_rectCursorText.width() && g_rectCursorText.height()) {
        //                pPainter->save();

        //                QFont font = pPainter->font();
        //                font.setBold(true);
        //                pPainter->setFont(font);

        //                pPainter->drawText(g_rectCursorText, g_sCursorText);

        //                pPainter->restore();
        //            }
        //        }
    }

    delete pPainter;

#ifdef QT_DEBUG
//    qDebug("Elapsed XAbstractTableView::paintEvent %lld", timer.elapsed());
#endif
}

void XAbstractTableView::reload(bool bUpdateData)
{
    adjust(bUpdateData);
    viewport()->update();
}

void XAbstractTableView::setTextFont(const QFont &font)
{
    const QFontMetricsF fm(font);
    g_nCharWidth = fm.maxWidth();  // mb TODO + nDelta TODO real
    g_nCharHeight = fm.height();

    g_fontText = font;

    adjustColumns();
    adjust();
    viewport()->update();
}

QFont XAbstractTableView::getTextFont()
{
    return g_fontText;
}

bool XAbstractTableView::setTextFontFromOptions(XOptions::ID id)
{
    bool bResult = false;

    QFont _font;
    QString sFont = getGlobalOptions()->getValue(id).toString();

    if (sFont != "") {
        if (_font.fromString(sFont)) {
            setTextFont(_font);
            bResult = true;
        } else {
            emit errorMessage(QString("%1: %2").arg(tr("Invalid font"), sFont));
        }
    }

    return bResult;
}

void XAbstractTableView::setTotalScrollCount(qint64 nValue)
{
    qint32 nScrollValue = 0;

    // TODO fix scroll for the large files
    // mb flag for large files
    // TODO nValue - 1
    if (nValue > getMaxScrollValue()) {
        nScrollValue = (qint32)getMaxScrollValue();
    } else {
        nScrollValue = (qint32)nValue;
    }

    verticalScrollBar()->setRange(0, nScrollValue);

    g_nTotalScrollCount = nValue;
}

quint64 XAbstractTableView::getTotalScrollCount()
{
    return g_nTotalScrollCount;
}

void XAbstractTableView::setLineDelta(qint32 nValue)
{
    g_nLineDelta = nValue;
}

void XAbstractTableView::setSideDelta(qint32 nValue)
{
    g_nSideDelta = nValue;
}

qint32 XAbstractTableView::getLinesProPage()
{
    return g_nLinesProPage;
}

void XAbstractTableView::setViewPosStart(qint64 nValue)
{
    g_nViewPosStart = nValue;
}

qint64 XAbstractTableView::getViewPosStart()
{
    return g_nViewPosStart;
}

qreal XAbstractTableView::getCharWidth()
{
    return g_nCharWidth;
}

XAbstractTableView::CURSOR_POSITION XAbstractTableView::getCursorPosition(const QPoint &pos)
{
    CURSOR_POSITION result = {};
    result.nY = pos.y();
    result.nX = pos.x() + g_nXViewPos;

    qint32 nHeaderHeight = (g_bHeaderVisible) ? (g_nHeaderHeight) : (0);

    qint32 nCurrentOffset = 0;
    qint32 nNumberOfColumns = g_listColumns.count();

    for (qint32 i = 0; i < nNumberOfColumns; i++) {
        if (g_listColumns.at(i).bEnable) {
            if ((result.nX >= nCurrentOffset) && (result.nX < (nCurrentOffset + g_listColumns.at(i).nWidth))) {
                result.bIsValid = true;
                result.nColumn = i;

                if (result.nY < nHeaderHeight) {
                    result.ptype = PT_HEADER;
                } else {
                    result.ptype = PT_CELL;
                    result.nRow = (result.nY - nHeaderHeight) / g_nLineHeight;
                    result.nAreaTop = (result.nY - nHeaderHeight) % (qint32)g_nLineHeight;
                    result.nAreaLeft = result.nX - nCurrentOffset;
                }

                if (result.nX >= (nCurrentOffset + g_listColumns.at(i).nWidth - g_nSideDelta)) {
                    if (!g_bColumnFixed) {
                        if (g_bVerticalLinesVisible) {
                            result.bResizeColumn = true;
                        } else {
                            if (result.nY < nHeaderHeight) {
                                result.bResizeColumn = true;
                            }
                        }

                        if ((result.nColumn == (nNumberOfColumns - 1)) && (g_bLastColumnStretch)) {
                            result.bResizeColumn = false;
                        }
                    }
                }

                break;
            }

            nCurrentOffset += g_listColumns.at(i).nWidth;
        }
    }

    if (isMapEnable()) {
        if ((result.nX >= (g_nViewWidth - g_nMapWidth)) && (result.nX < g_nViewWidth)) {
            result.bIsValid = true;

            if (result.nY < nHeaderHeight) {
                result.ptype = PT_MAPHEADER;
            } else {
                result.ptype = PT_MAP;
                result.nPercentage = ((result.nY - nHeaderHeight) * getMapCount()) / (g_nViewHeight - nHeaderHeight);
            }
        }
    }

    return result;
}

bool XAbstractTableView::isViewPosSelected(qint64 nViewPos)
{
    bool bResult = false;

    if ((nViewPos >= g_state.nSelectionViewPos) && (nViewPos < (g_state.nSelectionViewPos + g_state.nSelectionViewSize))) {
        bResult = true;
    }

    return bResult;
}

qint32 XAbstractTableView::getLineDelta()
{
    return g_nLineDelta;
}

qint32 XAbstractTableView::getSideDelta()
{
    return g_nSideDelta;
}

XAbstractTableView::STATE XAbstractTableView::getState()
{
    return g_state;
}

void XAbstractTableView::setState(STATE state)
{
    g_state = state;

    emit selectionChanged();
}

// qint64 XAbstractTableView::getCursorViewPos()
//{
//     return g_state.nCursorViewPos;
// }

// void XAbstractTableView::setCursorViewPos(qint64 nViewPos, qint32 nColumn, QVariant varCursorExtraInfo)
//{
//     if (nColumn != -1) {
//         g_state.cursorPosition.nColumn = nColumn;
//     }

//    g_state.varCursorExtraInfo = varCursorExtraInfo;

//    if (g_state.nCursorViewPos != nViewPos) {
//        g_state.nCursorViewPos = nViewPos;

//        emit cursorViewPosChanged(nViewPos);
//    }
//}

void XAbstractTableView::_initSelection(qint64 nViewPos, qint64 nSize)
{
    // #ifdef QT_DEBUG
    //     qDebug("_initSelection %llx, %llx", nViewPos, nSize);
    // #endif
    if (isViewPosValid(nViewPos) || isEnd(nViewPos)) {
        g_nSelectionInitOffset = nViewPos;
        g_nSelectionInitSize = nSize;
        g_state.nSelectionViewPos = nViewPos;
        g_state.nSelectionViewSize = 0;

        if (g_nMaxSelectionViewSize) {
            g_nSelectionInitSize = qMin(g_nSelectionInitSize, g_nMaxSelectionViewSize);
        }
    }
}

void XAbstractTableView::_setSelection(qint64 nViewPos, qint64 nSize)
{
    // #ifdef QT_DEBUG
    //     qDebug("_setSelection %llx, %llx", nViewPos, nSize);
    // #endif
    if (isViewPosValid(nViewPos) || isEnd(nViewPos)) {
        if (nViewPos > g_nSelectionInitOffset) {
            g_state.nSelectionViewPos = g_nSelectionInitOffset;
            g_state.nSelectionViewSize = nViewPos - g_nSelectionInitOffset + nSize;
        } else {
            g_state.nSelectionViewPos = nViewPos;
            g_state.nSelectionViewSize = g_nSelectionInitOffset - nViewPos + g_nSelectionInitSize;
        }

        if (g_nMaxSelectionViewSize) {
            g_state.nSelectionViewSize = qMin(g_state.nSelectionViewSize, g_nMaxSelectionViewSize - g_state.nSelectionViewPos);
        }

        if (g_state.nSelectionViewSize == 0) {
            g_state.nSelectionViewSize = 1;
        }

        emit selectionChanged();
    }
}

void XAbstractTableView::_initSetSelection(qint64 nViewPos, qint64 nSize)
{
    if (nViewPos != -1) {
        _initSelection(nViewPos, nSize);
        _setSelection(nViewPos, nSize);
    }
}

void XAbstractTableView::verticalScroll()
{
    g_nViewPosStart = getCurrentViewPosFromScroll();

    adjust(true);
}

void XAbstractTableView::horisontalScroll()
{
    adjust();
}

void XAbstractTableView::adjust(bool bUpdateData)
{
    if (isActive()) {
        {
            g_nViewWidth = viewport()->width();
            g_nViewHeight = viewport()->height();

            g_nLineHeight = g_nCharHeight + g_nLineDelta;

            qint32 nHeaderHeight = (g_bHeaderVisible) ? (g_nHeaderHeight) : (0);

            qint32 nLinesProPage = 0;

            if (g_nLineHeight) {
                nLinesProPage = (g_nViewHeight - nHeaderHeight) / g_nLineHeight;
            }

            if (nLinesProPage < 0) {
                nLinesProPage = 0;
            }

            if (g_nLinesProPage != nLinesProPage) {
                bUpdateData = true;
                g_nLinesProPage = nLinesProPage;
            }

            g_nTableWidth = 0;
            qint32 nNumberOfColumns = g_listColumns.count();

            if (g_bLastColumnStretch) {
                if (nNumberOfColumns) {
                    nNumberOfColumns--;
                }
            }

            for (qint32 i = 0; i < nNumberOfColumns; i++) {
                if (g_listColumns.at(i).bEnable) {
                    g_listColumns[i].nLeft = g_nTableWidth;
                    g_nTableWidth += g_listColumns.at(i).nWidth;
                }
            }

            if (isMapEnable()) {
                g_nTableWidth += getMapWidth();
            }

            qint32 nDelta = g_nTableWidth - g_nViewWidth;

            if (g_bLastColumnStretch) {
                //            nDelta+=3; // TODO Check
                // TODO !!!
                qint32 _nNumberOfColumns = g_listColumns.count();

                if (nDelta < 0) {
                    g_listColumns[_nNumberOfColumns - 1].nWidth = -(nDelta);
                } else {
                    g_listColumns[_nNumberOfColumns - 1].nWidth = 0;
                }
            }

            horizontalScrollBar()->setRange(0, nDelta);
            horizontalScrollBar()->setPageStep(g_nViewWidth);

            g_nXViewPos = horizontalScrollBar()->value();

            if (bUpdateData) {
#ifdef QT_DEBUG
                QElapsedTimer timer;
                timer.start();
#endif
                updateData();
#ifdef QT_DEBUG
//                qDebug("updateData %lld", timer.elapsed());
#endif
            }

            //    resetCursor(); // TODO Check
            // TODO
        }
    }
}

void XAbstractTableView::adjustColumns()
{
    // TODO
}

void XAbstractTableView::adjustHeader()
{
}

void XAbstractTableView::_headerClicked(qint32 nNumber)
{
    adjustHeader();

    emit headerClicked(nNumber);
}

void XAbstractTableView::_cellDoubleClicked(qint32 nRow, qint32 nColumn)
{
    emit cellDoubleClicked(nRow, nColumn);
}

qint64 XAbstractTableView::getFixViewPos(qint64 nViewPos)
{
    return nViewPos;
}

void XAbstractTableView::adjustMap()
{
}

// void XAbstractTableView::setCursorData(QRect rectSquare, QRect rectText, const QString &sText, qint32 nDelta)
//{
//     g_rectCursorSquare = rectSquare;
//     g_rectCursorText = rectText;
//     g_sCursorText = sText;
//     g_nCursorDelta = nDelta;
// }

// void XAbstractTableView::resetCursorData()
//{
//     setCursorData(QRect(), QRect(), "", 0);
// }

// qint32 XAbstractTableView::getCursorDelta()
//{
//     return g_nCursorDelta;
// }

qint64 XAbstractTableView::getMaxScrollValue()
{
    return 0x7FFFFFFF;
}

void XAbstractTableView::setLastColumnStretch(bool bState)
{
    g_bLastColumnStretch = bState;
}

void XAbstractTableView::setHeaderVisible(bool bState)
{
    g_bHeaderVisible = bState;
}

void XAbstractTableView::setColumnFixed(bool bState)
{
    g_bColumnFixed = bState;
}

void XAbstractTableView::setVerticalLinesVisible(bool bState)
{
    g_bVerticalLinesVisible = bState;
}

void XAbstractTableView::setHorisontalLinesVisible(bool bState)
{
    g_bHorisontalLinesVisible = bState;
}

qint64 XAbstractTableView::getSelectionInitOffset()
{
    return g_nSelectionInitOffset;
}

qint64 XAbstractTableView::getSelectionInitSize()
{
    return g_nSelectionInitSize;
}

void XAbstractTableView::setCurrentBlock(qint64 nViewPos, qint64 nSize)
{
    g_nCurrentBlockViewPos = nViewPos;
    g_nCurrentBlockViewSize = nSize;
}

bool XAbstractTableView::isViewPosInCurrentBlock(qint64 nViewPos)
{
    bool bResult = false;

    if (g_nCurrentBlockViewSize) {
        if ((g_nCurrentBlockViewPos <= nViewPos) && (nViewPos < (g_nCurrentBlockViewPos + g_nCurrentBlockViewSize))) {
            bResult = true;
        }
    }

    return bResult;
}

qreal XAbstractTableView::getLineHeight()
{
    return g_nLineHeight;
}

qreal XAbstractTableView::getHeaderHeight()
{
    return g_nHeaderHeight;
}

void XAbstractTableView::setHeaderHeight(qreal nHeight)
{
    g_nHeaderHeight = nHeight;
}

void XAbstractTableView::setSelectionEnable(bool bState)
{
    g_bIsSelectionEnable = bState;
}

void XAbstractTableView::setContextMenuEnable(bool bState)
{
    g_bIsContextMenuEnable = bState;
}

bool XAbstractTableView::isContextMenuEnable()
{
    return g_bIsContextMenuEnable;
}

// void XAbstractTableView::setBlinkingCursor(bool bState)
//{
//     if (bState) {
//         connect(&g_timerCursor, SIGNAL(timeout()), this, SLOT(updateBlink()));
//         g_timerCursor.setInterval(500);  // TODO Consts
//         g_timerCursor.start();
//     } else {
//         g_bBlink = true;

//        disconnect(&g_timerCursor, SIGNAL(timeout()), this, SLOT(updateBlink()));
//        g_timerCursor.stop();
//    }
//}

// void XAbstractTableView::setBlinkingCursorEnable(bool bState)
//{
//     if (g_bIsBlinkingCursorEnable != bState) {
//         g_bIsBlinkingCursorEnable = bState;

//        if (bState) {
//            if (isActive()) {
//                setBlinkingCursor(true);
//            }
//        } else {
//            setBlinkingCursor(false);
//        }
//    }
//}

void XAbstractTableView::_verticalScroll()
{
    verticalScroll();
}

bool XAbstractTableView::isSelectionEnable()
{
    return g_bIsSelectionEnable;
}

void XAbstractTableView::setMaxSelectionViewSize(qint64 nMaxSelectionViewSize)
{
    g_nMaxSelectionViewSize = nMaxSelectionViewSize;
}

void XAbstractTableView::setMapEnable(bool bState)
{
    g_bIsMapEnable = bState;
}

bool XAbstractTableView::isMapEnable()
{
    return g_bIsMapEnable;
}

void XAbstractTableView::setMapWidth(qint32 nWidth)
{
    g_nMapWidth = nWidth;
}

qreal XAbstractTableView::getMapWidth()
{
    return g_nMapWidth;
}

void XAbstractTableView::setMapCount(qint32 nMapCount)
{
    g_nMapCount = nMapCount;
}

qint32 XAbstractTableView::getMapCount()
{
    return g_nMapCount;
}

void XAbstractTableView::_customContextMenu(const QPoint &pos)
{
    contextMenu(mapToGlobal(pos));
}

// void XAbstractTableView::updateBlink()
//{
//     if (isFocused()) {
//         g_bBlink = (bool)(!g_bBlink);
//         viewport()->update(g_rectCursorSquare);
//     } else {
//         if (!g_bBlink) {
//             g_bBlink = true;
//             viewport()->update(g_rectCursorSquare);
//         }
//     }
// }

void XAbstractTableView::_copyValueSlot()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        QString sValue = pAction->property("VALUE").toString();

        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(sValue);
    }
}

void XAbstractTableView::resizeEvent(QResizeEvent *pEvent)
{
    if (isActive()) {
        adjust();
        QAbstractScrollArea::resizeEvent(pEvent);
    }
}

void XAbstractTableView::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());

        if (g_bIsSelectionEnable && g_bMouseSelection) {
            OS os = cursorPositionToOS(cursorPosition);

            if (os.nViewPos != -1) {
                //                g_state.nCursorOffset=os.nOffset;
                //                g_state.varCursorExtraInfo=os.varData;
                //                g_state.cursorPosition=cursorPosition;

                //                emit cursorChanged(os.nOffset);

                _setSelection(os.nViewPos, os.nSize);

                adjust();
                viewport()->update();
            }
        } else if (g_bMouseResizeColumn) {
            qreal nColumnWidth = qMax(g_nSideDelta, cursorPosition.nX - g_listColumns.at(g_nResizeColumnNumber).nLeft);

            g_listColumns[g_nResizeColumnNumber].nWidth = nColumnWidth;

            adjust();
            viewport()->update();
        } else if (pEvent->button() == Qt::NoButton) {
            if (cursorPosition.bResizeColumn) {
                setCursor(Qt::SplitHCursor);
            } else {
                unsetCursor();
            }
        }

        if (g_bHeaderClickButton) {
            g_bHeaderClickButton = ((cursorPosition.ptype == PT_HEADER) && (g_nHeaderClickColumnNumber == cursorPosition.nColumn));

            adjust();
            viewport()->update();
        }

        QAbstractScrollArea::mouseMoveEvent(pEvent);
    }
}

void XAbstractTableView::mousePressEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        if (pEvent->button() == Qt::LeftButton) {
            CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());
            OS os = cursorPositionToOS(cursorPosition);

            if (cursorPosition.ptype == PT_MAPHEADER) {
            } else if (cursorPosition.ptype == PT_MAP) {
                _goToViewPos(os.nViewPos, false, false, true);
            } else if (cursorPosition.bResizeColumn) {
                g_bMouseResizeColumn = true;
                g_nResizeColumnNumber = cursorPosition.nColumn;
                setCursor(Qt::SplitHCursor);
            } else if ((cursorPosition.ptype == PT_HEADER) && (g_listColumns.at(cursorPosition.nColumn).bClickable)) {
                g_bHeaderClickButton = true;
                g_nHeaderClickColumnNumber = cursorPosition.nColumn;
            } else if (os.nViewPos != -1) {
                //                g_state.nCursorViewPos = os.nViewPos;
                //                g_state.varCursorExtraInfo = os.varData;
                g_state.cursorPosition = cursorPosition;
                g_state.varCursorExtraInfo = os.varData;

                if (g_bIsSelectionEnable) {
                    _initSelection(os.nViewPos, os.nSize);
                    g_bMouseSelection = true;
                }

                emit cursorViewPosChanged(os.nViewPos);
            }

            adjust();
            viewport()->update();
        }

        QAbstractScrollArea::mousePressEvent(pEvent);
    }
}

void XAbstractTableView::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        if (pEvent->button() == Qt::LeftButton) {
            if (g_bHeaderClickButton) {
                _headerClicked(g_nHeaderClickColumnNumber);
            } else if (g_bIsSelectionEnable && g_bMouseSelection) {
                CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());
                OS os = cursorPositionToOS(cursorPosition);

                _setSelection(os.nViewPos, os.nSize);

                //            if(g_state.nCursorOffset==os.nOffset)
                //            {
                //                _setSelection(os.nOffset+os.nSize-1);
                //            }
            }

            adjust();
            viewport()->update();
        }

        g_bMouseResizeColumn = false;
        g_bHeaderClickButton = false;

        if (g_bIsSelectionEnable) {
            g_bMouseSelection = false;
        }
    }
}

void XAbstractTableView::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        if (pEvent->button() == Qt::LeftButton) {
            CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());
            OS os = cursorPositionToOS(cursorPosition);

            if ((cursorPosition.ptype == PT_HEADER) && (g_listColumns.at(cursorPosition.nColumn).bClickable)) {
                // TODO
            } else if (os.nViewPos != -1) {
                _cellDoubleClicked(cursorPosition.nRow, cursorPosition.nColumn);
            }

            //            adjust();
            //            viewport()->update();
        }
    }

    QAbstractScrollArea::mouseDoubleClickEvent(pEvent);
}

void XAbstractTableView::keyPressEvent(QKeyEvent *pEvent)
{
    QAbstractScrollArea::keyPressEvent(pEvent);
}

void XAbstractTableView::wheelEvent(QWheelEvent *pEvent)
{
    QAbstractScrollArea::wheelEvent(pEvent);
}

bool XAbstractTableView::isViewPosValid(qint64 nViewPos)
{
    Q_UNUSED(nViewPos)

    return false;
}

bool XAbstractTableView::isEnd(qint64 nViewPos)
{
    Q_UNUSED(nViewPos)

    return false;
}

XAbstractTableView::OS XAbstractTableView::cursorPositionToOS(const XAbstractTableView::CURSOR_POSITION &cursorPosition)
{
    Q_UNUSED(cursorPosition)

    OS result = {};

    return result;
}

void XAbstractTableView::updateData()
{
}

void XAbstractTableView::startPainting(QPainter *pPainter)
{
    Q_UNUSED(pPainter)
}

void XAbstractTableView::paintColumn(QPainter *pPainter, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight)
{
    Q_UNUSED(pPainter)
    Q_UNUSED(nColumn)
    Q_UNUSED(nLeft)
    Q_UNUSED(nTop)
    Q_UNUSED(nWidth)
    Q_UNUSED(nHeight)
}

void XAbstractTableView::paintMap(QPainter *pPainter, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight)
{
    Q_UNUSED(pPainter)
    Q_UNUSED(nLeft)
    Q_UNUSED(nTop)
    Q_UNUSED(nWidth)
    Q_UNUSED(nHeight)
}

void XAbstractTableView::paintCell(QPainter *pPainter, qint32 nRow, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight)
{
    Q_UNUSED(pPainter)
    Q_UNUSED(nRow)
    Q_UNUSED(nColumn)
    Q_UNUSED(nLeft)
    Q_UNUSED(nTop)
    Q_UNUSED(nWidth)
    Q_UNUSED(nHeight)
}

void XAbstractTableView::paintTitle(QPainter *pPainter, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight, const QString &sTitle)
{
    Q_UNUSED(nColumn)

    QRect rect = QRect(nLeft + 4, nTop, nWidth - 8, nHeight);
    pPainter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, sTitle);  // TODO alignment settings
}

void XAbstractTableView::endPainting(QPainter *pPainter)
{
    Q_UNUSED(pPainter)
}

bool XAbstractTableView::_goToViewPos(qint64 nViewPos, bool bSaveCursor, bool bShort, bool bAprox)
{
    Q_UNUSED(bSaveCursor)

    bool bResult = false;

    if (isViewPosValid(nViewPos)) {
        if (bAprox) {
            nViewPos = getFixViewPos(nViewPos);
        }

        //        qint64 nCursorOffset = 0;

        //        if (bSaveCursor) {
        //            nCursorOffset = getCursorViewPos();
        //        } else {
        //            nCursorOffset = nViewPos;
        //        }

        bool bScroll = true;

        if (bShort && isViewPosInCurrentBlock(nViewPos)) {
            bScroll = false;
        }

        if (bScroll) {
            setCurrentViewPosToScroll(nViewPos);
        }

        //        setCursorViewPos(nCursorOffset);

        bResult = true;
    }

    return bResult;
}

void XAbstractTableView::contextMenu(const QPoint &pos)
{
    Q_UNUSED(pos)
}

qint64 XAbstractTableView::getCurrentViewPosFromScroll()
{
    return verticalScrollBar()->value();
}

void XAbstractTableView::setCurrentViewPosToScroll(qint64 nViewPos)
{
    setViewPosStart(nViewPos);
    verticalScrollBar()->setValue((qint32)nViewPos);  // TODO Check large files
    adjust(true);
}
