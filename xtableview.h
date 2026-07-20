/* Copyright (c) 2024-2026 hors<horsicq@gmail.com>
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
#ifndef XTABLEVIEW_H
#define XTABLEVIEW_H

#include <QTableView>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QAtomicInt>
#include <QSharedPointer>
#include <QTimer>

#include "xheaderview.h"
#include "xsortfilterproxymodel.h"
#include "xmodel.h"
#include "xoptions.h"

class XTableView : public QTableView {
    Q_OBJECT

public:
    XTableView(QWidget *pParent = nullptr);
    ~XTableView();

    void setCustomModel(QAbstractItemModel *pModel, bool bFilterEnabled);
    void clear();
    XSortFilterProxyModel *getProxyModel();
    void setFilterEnabled(qint32 nColumn, bool bFilterEnabled);
    void setColumnFilterString(qint32 nColumn, const QString &sFilter);
    void adjust();
    bool isSortingEnabled() const;

    // Opt-in: run filtering/sorting on a worker thread and show progress via
    // busyChanged(). Off by default so every existing caller keeps today's
    // synchronous behavior unchanged. Changing a filter/sort while a previous
    // one is still computing cancels it and starts fresh.
    void setThreadedFilterSortEnabled(bool bEnabled);
    bool isThreadedFilterSortEnabled() const;

public slots:
    void setSortingEnabled(bool bEnable);
    void sortByColumn(int column, Qt::SortOrder order);

signals:
    void invalidateSignal();
    void busyChanged(bool bBusy);

private:
    void deleteOldModel(QAbstractItemModel **ppOldModel);
    void handleFilter();

    enum PENDING_OPERATION { OPERATION_NONE = 0, OPERATION_FILTER, OPERATION_SORT };

    void startAsyncFilterOperation(const QList<QString> &listFilters);
    void startAsyncCustomFilterOperation(const QList<QString> &listFilters);
    void startAsyncSortOperation(qint32 nColumn, Qt::SortOrder order);
    void cancelAsyncOperation(bool bWait = true);

private slots:
    void onFilterChanged();
    void onFilterApply();
    void onSortChanged(int column, Qt::SortOrder order);
    void horizontalScroll();
    void onAsyncOperationFinished();
    void onSourceModelReset();

private:
    XHeaderView *m_pHeaderView;
    XSortFilterProxyModel *m_pSortFilterProxyModel;
    QAbstractItemModel *m_pOldModel;
    QAbstractItemModel *m_pModel;
    bool m_bIsXmodel;
    bool m_bIsCustomFilter;
    bool m_bIsCustomSort;
    XModel *m_pXModel;
    QFutureWatcher<void> m_watcher;
    bool m_bIsStop;
    QTimer *m_pFilterTimer;
    QList<QString> m_listCurrentFilters;

    bool m_bThreadedEnabled;
    bool m_bSortingEnabled;
    bool m_bApplyingAsyncSort;
    QFutureWatcher<bool> *m_pAsyncWatcher;
    QSharedPointer<QAtomicInt> m_pAsyncCancelFlag;
    PENDING_OPERATION m_pendingOperation;
    QList<QString> m_listPendingFilters;
    qint32 m_nPendingSortColumn;
    Qt::SortOrder m_pendingSortOrder;
    qint32 m_nCustomFilterGeneration;
};

#endif  // XTABLEVIEW_H
