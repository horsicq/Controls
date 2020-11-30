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
#ifndef XABSTRACTTABLEVIEW_H
#define XABSTRACTTABLEVIEW_H

#include <QAbstractScrollArea>
#include <QStyleOptionButton>
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QMessageBox>
#include <QMenu>
#include <QShortcut>
#include <QFileDialog>
#include "xshortcuts.h"

class XAbstractTableView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    struct COLUMN
    {
        qint32 nWidth;
        QString sTitle;
    };

    enum PT
    {
        PT_UNKNOWN=0,
        PT_HEADER,
        PT_CELL
    };

    struct CURSOR_POSITION
    {
        bool bIsValid;
        PT ptype;
        qint32 nRow;
        qint32 nColumn;
        qint32 nCellTop;
        qint32 nCellLeft;
    };

    struct STATE
    {
        qint64 nSelectionOffset;
        qint64 nSelectionSize;
    };

    explicit XAbstractTableView(QWidget *pParent=nullptr);
    ~XAbstractTableView();

    void addColumn(qint32 nWidth,QString sTitle);
    void reload(bool bUpdateData=false);

    void setTextFont(const QFont &font);
    QFont getTextFont();

    void setTotalLineCount(qint64 nValue);
    quint64 getTotalLineCount();

    qint32 getLinesProPage();
    qint64 getViewStart();

    qint32 getCharWidth();

    CURSOR_POSITION getCursorPosition(QPoint pos);

    bool isOffsetSelected(qint64 nOffset);

    QPainter *getPainter();
    QPainter *getBoldTextPointer();

    qint32 getLineDelta();

    STATE getState();

private:
    void _initSelection(qint64 nOffset);
    void _setSelection(qint64 nOffset);

private slots:
    void verticalScroll();
    void horisontalScroll();
    void adjust(bool bUpdateData=false);
    void _customContextMenu(const QPoint &pos);

protected:
    virtual void paintEvent(QPaintEvent* pEvent) override;
    virtual void resizeEvent(QResizeEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void keyPressEvent(QKeyEvent *pEvent);
    virtual void wheelEvent(QWheelEvent *pEvent);

    virtual bool isOffsetValid(qint64 nOffset)=0;
    virtual qint64 cursorPositionToOffset(CURSOR_POSITION cursorPosition)=0;
    virtual void updateData()=0;
    virtual void startPainting()=0;
    virtual void paintColumn(qint32 nColumn,qint32 nLeft,qint32 nTop,qint32 nWidth,qint32 nHeight)=0;
    virtual void paintCell(qint32 nRow,qint32 nColumn,qint32 nLeft,qint32 nTop,qint32 nWidth,qint32 nHeight)=0;
    virtual void endPainting()=0;
    virtual void goToOffset(qint64 nOffset)=0;
    virtual void contextMenu(const QPoint &pos){Q_UNUSED(pos)}

private:
    qint64 g_nNumberOfRows;
    QList<COLUMN> g_listColumns;
    bool g_bShowLines;
    qint32 g_nHeaderHeight;
    QPushButton pushButtonHeader; // TODO
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

    QPainter *g_pPainter;

    STATE g_state;
    bool g_bMouseSelection;
    qint64 g_nSelectionInitOffset;
};

#endif // XABSTRACTTABLEVIEW_H
