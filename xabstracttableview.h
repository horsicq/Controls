/* Copyright (c) 2020-2022 hors<horsicq@gmail.com>
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
#ifndef XABSTRACTTABLEVIEW_H
#define XABSTRACTTABLEVIEW_H

#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>
#include <QStyleOptionButton>
#include <QTimer>
#include "xshortcutstscrollarea.h"

class XAbstractTableView : public XShortcutstScrollArea
{
    Q_OBJECT

public:
    struct COLUMN
    {
        bool bEnable;
        qint32 nLeft;
        qint32 nWidth;
        QString sTitle;
        bool bClickable;
    };

    enum PT
    {
        PT_UNKNOWN=0,
        PT_HEADER,
        PT_CELL,
    };

    struct CURSOR_POSITION
    {
        bool bIsValid;
        PT ptype;
        qint32 nY;
        qint32 nX;
        qint32 nRow;
        qint32 nColumn;
        qint32 nCellTop;
        qint32 nCellLeft;
        bool bResizeColumn;
    };

    struct STATE
    {
        qint64 nCursorOffset;
        QVariant varCursorExtraInfo;
        qint64 nSelectionOffset;
        qint64 nSelectionSize;
        CURSOR_POSITION cursorPosition;
    };

    struct OS
    {
        qint64 nOffset;
        qint64 nSize;
        QVariant varData;
    };

    explicit XAbstractTableView(QWidget *pParent=nullptr);
    ~XAbstractTableView();

    void setActive(bool bIsActive);
    bool isActive();
//    void setEnabled(bool bEnabled);
    void clear();

    void addColumn(QString sTitle,qint32 nWidth=0,bool bClickable=false,bool bEnable=true);
    void setColumnTitle(qint32 nNumber,QString sTitle);
    void setColumnEnabled(qint32 nNumber,bool bState);
    void setColumnWidth(qint32 nNumber,qint32 nWidth);

    void reload(bool bUpdateData=false);

    void setTextFont(const QFont &font);
    QFont getTextFont();

    void setTotalLineCount(qint64 nValue);
    quint64 getTotalLineCount();

    void setLineDelta(qint32 nValue);
    qint32 getLinesProPage();

    void setViewStart(qint64 nValue);
    qint64 getViewStart();

    qint32 getCharWidth();
    static qint32 getCharWidth(QWidget *pWidget);
    static qint32 getCharHeight(QWidget *pWidget);

    CURSOR_POSITION getCursorPosition(QPoint pos);

    bool isOffsetSelected(qint64 nOffset);
    QPainter *getBoldTextPointer();

    qint32 getLineDelta();
    STATE getState();
    qint64 getCursorOffset();
    void setCursorOffset(qint64 nValue,qint32 nColumn=-1,QVariant varCursorExtraInfo=QVariant());
    void adjust(bool bUpdateData=false);

    void setCursorData(QRect rectSquare,QRect rectText,QString sText,qint32 nDelta);
    void resetCursorData();

    qint32 getCursorDelta();
    void setSelection(qint64 nOffset,qint64 nSize);

    qint64 getMaxScrollValue();
    void setLastColumnStretch(bool bState);
    void setHeaderVisible(bool bState);
    void setColumnFixed(bool bState);
    void setVerticalLinesVisible(bool bState);
    void setHorisontalLinesVisible(bool bState);

    static QFont getMonoFont(qint32 nFontSize=-1);
    qint64 getSelectionInitOffset();
    void setCurrentBlock(qint64 nOffset,qint64 nSize);
    bool isOffsetInCurrentBlock(qint64 nOffset);
    qint32 getLineHeight();
    qint32 getHeaderHeight();
    void setSelectionEnable(bool bState);

signals:
    void cursorChanged(qint64 nOffset);
    void selectionChanged();
    void errorMessage(QString sText);
    void infoMessage(QString sText);
    void editState(bool bState);
    void headerClicked(qint32 nNumber);
    void cellDoubleClicked(qint32 nRow,qint32 nColumn);

private:
    void _initSelection(qint64 nOffset);
    void _setSelection(qint64 nOffset);

private slots:
    void verticalScroll();
    void horisontalScroll();
    void _customContextMenu(const QPoint &pos);
    void updateBlink();

protected:
    virtual void paintEvent(QPaintEvent* pEvent) override;
    virtual void resizeEvent(QResizeEvent *pEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *pEvent) override;
    virtual void keyPressEvent(QKeyEvent *pEvent) override;
    virtual void wheelEvent(QWheelEvent *pEvent) override;
    virtual bool isOffsetValid(qint64 nOffset);
    virtual bool isEnd(qint64 nOffset);
    virtual OS cursorPositionToOS(CURSOR_POSITION cursorPosition);
    virtual void updateData();
    virtual void startPainting(QPainter *pPainter);
    virtual void paintColumn(QPainter *pPainter,qint32 nColumn,qint32 nLeft,qint32 nTop,qint32 nWidth,qint32 nHeight);
    virtual void paintCell(QPainter *pPainter,qint32 nRow,qint32 nColumn,qint32 nLeft,qint32 nTop,qint32 nWidth,qint32 nHeight);
    virtual void endPainting(QPainter *pPainter);
    virtual bool _goToOffset(qint64 nOffset,bool bSaveCursor=false,bool bShort=false);
    virtual void contextMenu(const QPoint &pos);
    virtual qint64 getScrollValue();
    virtual void setScrollValue(qint64 nOffset);
    virtual void adjustColumns();
    virtual void _headerClicked(qint32 nNumber);
    virtual void _cellDoubleClicked(qint32 nRow,qint32 nColumn);

private:
    bool g_bIsActive;
    qint64 g_nNumberOfRows;
    QList<COLUMN> g_listColumns;
    qint32 g_nHeaderHeight;
    QPushButton g_pushButtonHeader;
    qint32 g_nXOffset;
    qint64 g_nViewStart;
    qint32 g_nCharWidth;
    qint32 g_nCharHeight;
    qint32 g_nLinesProPage;
    qint32 g_nLineHeight;
    QFont g_fontText;
    qint64 g_nTotalLineCount;
    qint32 g_nViewWidth;
    qint32 g_nViewHeight;
    qint32 g_nTableWidth;

    qint32 g_nLineDelta;

    STATE g_state;
    bool g_bMouseSelection;
    qint64 g_nSelectionInitOffset;
    bool g_bMouseResizeColumn;
    qint32 g_nResizeColumnNumber;
    bool g_bHeaderClickButton;
    qint32 g_nHeaderClickColumnNumber;

    QTimer g_timerCursor;
    QRect g_rectCursorSquare;
    QRect g_rectCursorText;
    QString g_sCursorText;
    qint32 g_nCursorDelta;
    bool g_bBlink;

    bool g_bLastColumnStretch;
    bool g_bHeaderVisible;
    bool g_bColumnFixed;
    bool g_bVerticalLinesVisible;
    bool g_bHorisontalLinesVisible;

    qint64 g_nCurrentBlockOffset;
    qint64 g_nCurrentBlockSize;

    bool g_bIsSelectionEnable;
};

#endif // XABSTRACTTABLEVIEW_H
