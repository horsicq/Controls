/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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

#ifndef XFTABLEVIEW_H
#define XFTABLEVIEW_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QProgressBar>
#include <QStatusBar>
#include "xtableview.h"
#include "xfmodel_header.h"
#include "xfmodel_table.h"
#include "xformats.h"

class XFTableView : public QWidget {
    Q_OBJECT

public:
    explicit XFTableView(QWidget *pParent = nullptr);
    virtual ~XFTableView();

    void setData(const XBinary::INDATA &inData, const XBinary::XFHEADER &xfHeader);
    void setCustomModel(QAbstractItemModel *pModel, bool bFilterEnabled);
    void clear();
    void adjust();
    QAbstractItemModel *model() const;
    QItemSelectionModel *selectionModel() const;

    void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior);
    void setSelectionMode(QAbstractItemView::SelectionMode mode);
    void setEditTriggers(QAbstractItemView::EditTriggers triggers);
    void setSortingEnabled(bool bEnable);
    QHeaderView *horizontalHeader() const;
    QHeaderView *verticalHeader() const;
    void setThreadedFilterSortEnabled(bool bEnabled);
    bool isThreadedFilterSortEnabled() const;
    void setShowOffset(bool bShowOffset);
    void setShowPresentation(bool bShowPresentation);
    void setStretchLastColumn(bool bState);

    // Resize to fit contents, but a column with the stretched last section is
    // left alone (Qt would just re-stretch it afterward), and the header title
    // itself is taken into account, not only the cell contents.
    void resizeColumnToContents(int column);
    void resizeColumnsToContents();

    void setProgressVisible(bool bVisible);
    void setProgressRange(qint32 nMinimum, qint32 nMaximum);
    void setProgressValue(qint32 nValue);
    void setProgressFormat(const QString &sFormat);

    void setStatusBarText(const QString &sText);

signals:
    void fieldSelected(qint32 nFieldIndex, QVariant value, const XBinary::XFRECORD &xfRecord);
    void fieldDoubleClicked(qint32 nFieldIndex, QVariant value, const XBinary::XFRECORD &xfRecord);
    void rowSelected(qint32 nRow);
    void busyChanged(bool bBusy);

private slots:
    void onCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenuRequested(const QPoint &pos);
    void onSaveClicked();
    void onFilterChanged();
    void onSortIndicatorChanged(int nColumn, Qt::SortOrder order);
    void onResetFilterClicked();
    void onTableBusyChanged(bool bBusy);

private:
    void updateResetFilterEnabled();

private:
    struct PROGRESS_STATE {
        bool bVisible;
        qint32 nMinimum;
        qint32 nMaximum;
        qint32 nValue;
        QString sFormat;
    };

    XBinary::INDATA m_inData;
    XBinary::XFHEADER m_xfHeader;
    XFModel_header *m_pHeaderModel;
    XFModel_table *m_pTableModel;

    XTableView *m_pTableView;
    QToolBar *m_pToolBar;
    QAction *m_pActionResetFilter;
    QAction *m_pActionSave;
    QProgressBar *m_pProgressBar;
    QStatusBar *m_pStatusBar;
    PROGRESS_STATE m_progressStateBeforeBusy;
    bool m_bSortActive;
    bool m_bProgressStateSaved;
};

#endif  // XFTABLEVIEW_H
