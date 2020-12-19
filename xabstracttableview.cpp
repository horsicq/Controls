// copyright (c) 2020 hors<horsicq@gmail.com>
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
#include "xabstracttableview.h"

XAbstractTableView::XAbstractTableView(QWidget *pParent) : QAbstractScrollArea(pParent)
{
    g_bMouseResizeColumn=false;
    g_bMouseSelection=false;
    g_pPainter=nullptr;
    g_nViewStart=0;
    g_nCharWidth=0;
    g_nCharHeight=0;
    g_nLinesProPage=0;
    g_nLineHeight=0;
    g_nTotalLineCount=0;
    g_nViewWidth=0;
    g_nViewHeight=0;
    g_nTableWidth=0;
    g_nSelectionInitOffset=-1;
    g_nNumberOfRows=0;
    g_nXOffset=0;
    g_bShowLines=true;
    g_nHeaderHeight=20;
    g_nLineDelta=4; // TODO Check
    g_state={};
    g_bBlink=false;

    g_nInitColumnNumber=0;

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(_customContextMenu(QPoint)));

    connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(verticalScroll()));
    connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(horisontalScroll()));

    connect(&g_timerCursor,SIGNAL(timeout()),this,SLOT(updateBlink()));

    g_timerCursor.setInterval(500);
    g_timerCursor.start();

    setMouseTracking(true); // Important
}

XAbstractTableView::~XAbstractTableView()
{

}

void XAbstractTableView::addColumn(qint32 nWidth, QString sTitle)
{
    COLUMN column={};

    column.bEnable=true;
    column.nWidth=nWidth;
    column.sTitle=sTitle;

    g_listColumns.append(column);
}

void XAbstractTableView::setColumnEnabled(qint32 nNumber, bool bState)
{
    if(nNumber<g_listColumns.count())
    {
        g_listColumns[nNumber].bEnable=bState;
    }
}

void XAbstractTableView::setColumnWidth(qint32 nNumber, qint32 nWidth)
{
    if(nNumber<g_listColumns.count())
    {
        g_listColumns[nNumber].nWidth=nWidth;
    }
}

void XAbstractTableView::paintEvent(QPaintEvent *pEvent)
{
    g_pPainter=new QPainter(this->viewport());
    g_pPainter->setFont(g_fontText);
    g_pPainter->setPen(viewport()->palette().color(QPalette::WindowText));
    g_pPainter->setBackgroundMode(Qt::TransparentMode);

    if(g_rectCursor!=pEvent->rect())
    {
        startPainting();

        int nNumberOfColumns=g_listColumns.count();

        qint32 nTopLeftY=pEvent->rect().topLeft().y();
        qint32 nTopLeftX=pEvent->rect().topLeft().x()-g_nXOffset;
        qint32 nScreenWidth=pEvent->rect().width();

        qint32 nHeight=pEvent->rect().height();

        qint32 nX=nTopLeftX;

        qint32 nHeaderHeight=g_nHeaderHeight;

        for(int i=0;i<nNumberOfColumns;i++)
        {
            if(g_listColumns.at(i).bEnable)
            {
                qint32 nColumnWidth=g_listColumns.at(i).nWidth;

                getPainter()->fillRect(nX,nTopLeftY+nHeaderHeight,nColumnWidth,nHeight-nHeaderHeight,viewport()->palette().color(QPalette::Base));

                paintColumn(i,nX,nTopLeftY+nHeaderHeight,nColumnWidth,nHeight-nHeaderHeight);

                for(int j=0;j<g_nLinesProPage;j++)
                {
                    paintCell(j,i,nX,nTopLeftY+nHeaderHeight+(j*g_nLineHeight),nColumnWidth,g_nLineHeight);
                }

                nX+=nColumnWidth;
            }
        }

        // Rest
        getPainter()->fillRect(nX,nTopLeftY,nScreenWidth-nX,nHeight,viewport()->palette().color(QPalette::Base));

        nX=nTopLeftX;

        // Draw lines and headers
        for(int i=0;i<nNumberOfColumns;i++)
        {
            if(g_listColumns.at(i).bEnable)
            {
                qint32 nColumnWidth=g_listColumns.at(i).nWidth;

                if(nHeaderHeight>0)
                {
                    QStyleOptionButton styleOptionButton;
                    styleOptionButton.state=QStyle::State_Enabled;

                    styleOptionButton.rect=QRect(nX,nTopLeftY,nColumnWidth,nHeaderHeight);

                    pushButtonHeader.style()->drawControl(QStyle::CE_PushButton,&styleOptionButton,g_pPainter,&pushButtonHeader);

                    QRect rect=QRect(nX+4,nTopLeftY,nColumnWidth-8,nHeaderHeight);
                    g_pPainter->drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,g_listColumns.at(i).sTitle); // TODO alignment
                }

                if(g_bShowLines)
                {
                    g_pPainter->drawLine(nX+nColumnWidth,nTopLeftY+nHeaderHeight,nX+nColumnWidth,nTopLeftY+nHeight);
                }

                nX+=nColumnWidth;
            }
        }

        endPainting();
    }

    // Draw cursor
    if(g_rectCursor.width()&&g_rectCursor.height())
    {
        // TODO bold
        QRect rectSelected;
        rectSelected.setRect(g_rectCursor.x(),g_rectCursor.y(),g_rectCursor.width(),g_rectCursor.height());

        if(g_bBlink&&hasFocus())
        {
            g_pPainter->setPen(viewport()->palette().color(QPalette::Highlight));
            g_pPainter->fillRect(rectSelected,this->palette().color(QPalette::WindowText));
        }
        else
        {
            g_pPainter->setPen(viewport()->palette().color(QPalette::WindowText));
            g_pPainter->fillRect(rectSelected,this->palette().color(QPalette::Base));
        }

        g_pPainter->drawText(g_rectCursor.x(),g_rectCursor.y()+g_nLineHeight-g_nLineDelta,g_sCursorText);
    }

    delete g_pPainter;
}

void XAbstractTableView::reload(bool bUpdateData)
{
    adjust(bUpdateData);
    viewport()->update();
}

void XAbstractTableView::setTextFont(const QFont &font)
{
    const QFontMetricsF fm(font);
    g_nCharWidth=fm.boundingRect('2').width();
    g_nCharWidth=qMax(fm.boundingRect('W').width(),(qreal)g_nCharWidth);
    g_nCharHeight=fm.height();

    g_fontText=font;

    adjust();
    viewport()->update();
}

QFont XAbstractTableView::getTextFont()
{
    return g_fontText;
}

void XAbstractTableView::setTotalLineCount(qint64 nValue)
{
    qint32 nScrollValue=0;

    // TODO fix scroll for the large files
    // mb flag for large files
    if(nValue>getMaxScrollValue())
    {
        nScrollValue=(qint32)getMaxScrollValue();
    }
    else
    {
        nScrollValue=(qint32)nValue;
    }

    verticalScrollBar()->setRange(0,nScrollValue);

    g_nTotalLineCount=nValue;
}

quint64 XAbstractTableView::getTotalLineCount()
{
    return g_nTotalLineCount;
}

qint32 XAbstractTableView::getLinesProPage()
{
    return g_nLinesProPage;
}

void XAbstractTableView::setViewStart(qint64 nValue)
{
    g_nViewStart=nValue;
}

qint64 XAbstractTableView::getViewStart()
{
    return g_nViewStart;
}

qint32 XAbstractTableView::getCharWidth()
{
    return g_nCharWidth;
}

XAbstractTableView::CURSOR_POSITION XAbstractTableView::getCursorPosition(QPoint pos)
{
    CURSOR_POSITION result={};

    result.nY=pos.y();
    result.nX=pos.x();

    qint32 nCurrentOffset=0;
    int nNumberOfColumns=g_listColumns.count();

    for(int i=0;i<nNumberOfColumns;i++)
    {
        if(g_listColumns.at(i).bEnable)
        {
            if((result.nX>=nCurrentOffset)&&(result.nX<(nCurrentOffset+g_listColumns.at(i).nWidth)))
            {
                result.bIsValid=true;
                result.nColumn=i;

                if(result.nY<g_nHeaderHeight)
                {
                    result.ptype=PT_HEADER;
                }
                else
                {
                    result.ptype=PT_CELL;
                    result.nRow=(result.nY-g_nHeaderHeight)/g_nLineHeight;
                    result.nCellTop=(result.nY-g_nHeaderHeight)%g_nLineHeight;
                    result.nCellLeft=result.nX-nCurrentOffset;
                }

                if(result.nX>=(nCurrentOffset+g_listColumns.at(i).nWidth-g_nLineDelta))
                {
                    result.bResizeColumn=true;
                }

                break;
            }

            nCurrentOffset+=g_listColumns.at(i).nWidth;
        }
    }

    return result;
}

bool XAbstractTableView::isOffsetSelected(qint64 nOffset)
{
    bool bResult=false;

    if((nOffset>=g_state.nSelectionOffset)&&(nOffset<(g_state.nSelectionOffset+g_state.nSelectionSize)))
    {
        bResult=true;
    }

    return bResult;
}

QPainter *XAbstractTableView::getPainter()
{
    return g_pPainter;
}

qint32 XAbstractTableView::getLineDelta()
{
    return g_nLineDelta;
}

XAbstractTableView::STATE XAbstractTableView::getState()
{
    return g_state;
}

qint64 XAbstractTableView::getCursorOffset()
{
    return g_state.nCursorOffset;
}

void XAbstractTableView::setCursorOffset(qint64 nValue)
{
    g_state.nCursorOffset=nValue;
}

void XAbstractTableView::_initSelection(qint64 nOffset)
{
    if(isOffsetValid(nOffset)||isEnd(nOffset))
    {
        g_nSelectionInitOffset=nOffset;
        g_state.nSelectionOffset=nOffset;
        g_state.nSelectionSize=0;
    }
}

void XAbstractTableView::_setSelection(qint64 nOffset)
{
    if(isOffsetValid(nOffset)||isEnd(nOffset))
    {
        if(nOffset>g_nSelectionInitOffset)
        {
            g_state.nSelectionOffset=g_nSelectionInitOffset;
            g_state.nSelectionSize=nOffset-g_nSelectionInitOffset;
        }
        else
        {
            g_state.nSelectionOffset=nOffset;
            g_state.nSelectionSize=g_nSelectionInitOffset-nOffset;
        }
    }
}

void XAbstractTableView::verticalScroll()
{
    g_nViewStart=getScrollValue();

    adjust(true);
}

void XAbstractTableView::horisontalScroll()
{
    adjust();
}

void XAbstractTableView::adjust(bool bUpdateData)
{
    g_nViewWidth=viewport()->width();
    g_nViewHeight=viewport()->height();

    g_nLineHeight=g_nCharHeight+5;

    qint32 nLinesProPage=(g_nViewHeight-g_nHeaderHeight)/g_nLineHeight;

    if(nLinesProPage<0)
    {
        nLinesProPage=0;
    }

    if(g_nLinesProPage!=nLinesProPage)
    {
        bUpdateData=true;
        g_nLinesProPage=nLinesProPage;
    }

    g_nTableWidth=0;
    int nNumberOfColumns=g_listColumns.count();

    for(int i=0;i<nNumberOfColumns;i++)
    {
        if(g_listColumns.at(i).bEnable)
        {
            g_listColumns[i].nLeft=g_nTableWidth;
            g_nTableWidth+=g_listColumns.at(i).nWidth;
        }
    }

    qint32 nDelta=g_nTableWidth-g_nViewWidth;

    // TODO set last column width

//    if(nDelta<=0)
//    {
//        int nNumberOfColumns=g_listColumns.count();

//        if(nNumberOfColumns)
//        {
//            g_listColumns[nNumberOfColumns-1].nWidth-=(nDelta);
//        }

//        nDelta=-1;
//    }

    horizontalScrollBar()->setRange(0,nDelta);
    horizontalScrollBar()->setPageStep(g_nViewWidth);

    g_nXOffset=horizontalScrollBar()->value();

    if(bUpdateData)
    {
        updateData();
    }

//    resetCursor(); // TODO Check

    // TODO
}

void XAbstractTableView::setCursorData(QRect rect, QString sText, qint32 nDelta)
{
    g_rectCursor=rect;
    g_sCursorText=sText;
    g_nCursorDelta=nDelta;
}

void XAbstractTableView::resetCursorData()
{
    setCursorData(QRect(),"",0);
}

qint32 XAbstractTableView::getCursorDelta()
{
    return g_nCursorDelta;
}

void XAbstractTableView::setSelection(qint64 nOffset, qint64 nSize)
{
    _initSelection(nOffset);
    _setSelection(nOffset+nSize);

    adjust();
    viewport()->update();
}

qint64 XAbstractTableView::getMaxScrollValue()
{
    return 0x7FFFFFFF;
}

void XAbstractTableView::_customContextMenu(const QPoint &pos)
{
    contextMenu(mapToGlobal(pos));
}

void XAbstractTableView::updateBlink()
{
    g_bBlink=(bool)(!g_bBlink);
    viewport()->update(g_rectCursor);
}

void XAbstractTableView::resizeEvent(QResizeEvent *pEvent)
{
    adjust();
    QAbstractScrollArea::resizeEvent(pEvent);
}

void XAbstractTableView::mouseMoveEvent(QMouseEvent *pEvent)
{
    CURSOR_POSITION cursorPosition=getCursorPosition(pEvent->pos());

    if(g_bMouseSelection)
    {
        qint64 nOffset=cursorPositionToOffset(cursorPosition);

        if(nOffset!=-1)
        {
            g_state.nCursorOffset=nOffset;
            g_state.cursorPosition=cursorPosition;
            _setSelection(nOffset);

            adjust();
            viewport()->update();
        }
    }
    else if(g_bMouseResizeColumn)
    {
        qint32 nColumnWidth=qMax(g_nLineDelta,cursorPosition.nX-g_listColumns.at(g_nInitColumnNumber).nLeft);

        g_listColumns[g_nInitColumnNumber].nWidth=nColumnWidth;

        adjust();
        viewport()->update();
    }
    else if(pEvent->button()==Qt::NoButton)
    {
        if(cursorPosition.bResizeColumn)
        {
            setCursor(Qt::SplitHCursor);
        }
        else
        {
            unsetCursor();
        }
    }

    QAbstractScrollArea::mouseMoveEvent(pEvent);
}

void XAbstractTableView::mousePressEvent(QMouseEvent *pEvent)
{
    if(pEvent->button()==Qt::LeftButton)
    {
        CURSOR_POSITION cursorPosition=getCursorPosition(pEvent->pos());
        qint64 nOffset=cursorPositionToOffset(cursorPosition);

        if(cursorPosition.bResizeColumn)
        {
            g_bMouseResizeColumn=true;
            g_nInitColumnNumber=cursorPosition.nColumn;
            setCursor(Qt::SplitHCursor);
        }
        else if(nOffset!=-1)
        {
            g_state.nCursorOffset=nOffset;
            g_state.cursorPosition=cursorPosition;
            _initSelection(nOffset);
            g_bMouseSelection=true;
        }

        adjust();
        viewport()->update();
    }

    QAbstractScrollArea::mousePressEvent(pEvent);
}

void XAbstractTableView::mouseReleaseEvent(QMouseEvent *pEvent)
{
    Q_UNUSED(pEvent)

    g_bMouseResizeColumn=false;
    g_bMouseSelection=false;
}

void XAbstractTableView::keyPressEvent(QKeyEvent *pEvent)
{
    QAbstractScrollArea::keyPressEvent(pEvent);
}

void XAbstractTableView::wheelEvent(QWheelEvent *pEvent)
{
    QAbstractScrollArea::wheelEvent(pEvent);
}

bool XAbstractTableView::goToOffset(qint64 nOffset)
{
    bool bResult=false;

    if(isOffsetValid(nOffset))
    {
        setScrollValue(nOffset);
        bResult=true;
    }

    return bResult;
}

qint64 XAbstractTableView::getScrollValue()
{
    return verticalScrollBar()->value();
}

void XAbstractTableView::setScrollValue(qint64 nOffset)
{
    verticalScrollBar()->setValue((qint32)nOffset);
}