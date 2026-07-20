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
#include "xtableview.h"
#include <QPointer>
#include <QSignalBlocker>
#include <QtConcurrent>

XTableView::XTableView(QWidget *pParent) : QTableView(pParent)
{
    m_pOldModel = nullptr;
    m_pModel = nullptr;
    m_pHeaderView = new XHeaderView(this);
    m_pSortFilterProxyModel = new XSortFilterProxyModel(this);
    m_bIsXmodel = false;
    m_bIsCustomFilter = false;
    m_bIsCustomSort = false;
    m_pXModel = nullptr;
    m_bIsStop = false;

    m_bThreadedEnabled = false;
    m_bSortingEnabled = true;
    m_bApplyingAsyncSort = false;
    m_pAsyncWatcher = nullptr;
    m_pendingOperation = OPERATION_NONE;
    m_nPendingSortColumn = -1;
    m_pendingSortOrder = Qt::AscendingOrder;
    m_nCustomFilterGeneration = 0;

    setHorizontalHeader(m_pHeaderView);

    connect(m_pHeaderView, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));
    connect(m_pHeaderView, SIGNAL(sortRequested(int, Qt::SortOrder)), this, SLOT(onSortChanged(int, Qt::SortOrder)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll()));
    connect(this, SIGNAL(invalidateSignal()), m_pSortFilterProxyModel, SLOT(invalidate()));

    m_pFilterTimer = new QTimer(this);
    m_pFilterTimer->setSingleShot(true);
    connect(m_pFilterTimer, SIGNAL(timeout()), this, SLOT(onFilterApply()));

    setSortingEnabled(true);
    setWordWrap(false);
    verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());
}

XTableView::~XTableView()
{
#ifdef QT_DEBUG
    qDebug("XTableView::~XTableView(): Closing window");
#endif

    m_bIsStop = true;
    m_nCustomFilterGeneration++;
    m_watcher.waitForFinished();
    cancelAsyncOperation();
    m_pSortFilterProxyModel->setSourceModel(nullptr);
    setModel(nullptr);
    deleteOldModel(&m_pModel);
}

void XTableView::setCustomModel(QAbstractItemModel *pModel, bool bFilterEnabled)
{
    // TODO Stretch last section
    m_nCustomFilterGeneration++;
    cancelAsyncOperation();

    m_pOldModel = m_pModel;

    if (m_pOldModel) {
        m_pSortFilterProxyModel->setSourceModel(nullptr);
        setModel(nullptr);
        // #ifdef QT_CONCURRENT_LIB
        // #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        //         QtConcurrent::run(&XTableView::deleteOldModel, this, &m_pOldModel);
        // #else
        //         QtConcurrent::run(this, &XTableView::deleteOldModel, &m_pOldModel);
        // #endif
        // #else
        //         deleteOldModel(&m_pOldModel);
        // #endif
        // m_pOldModel->beginResetModel();
#ifdef QT_DEBUG
        // get elapsed time
        QElapsedTimer timer;
        timer.start();
        qDebug("m_pOldModel->clear() START");
        // QStandartItemModel 433543
        // 470701 ms
#endif
        // m_pOldModel->clear();
        deleteOldModel(&m_pOldModel);
#ifdef QT_DEBUG
        qDebug("setCustomModel Elapsed time: %lld ms", timer.elapsed());
#endif
        // m_pOldModel->endResetModel();
    }

    m_pModel = pModel;

    if (pModel && bFilterEnabled) {
        m_pHeaderView->setNumberOfFilters(pModel->columnCount());
        m_pSortFilterProxyModel->setSourceModel(pModel);
        setModel(m_pSortFilterProxyModel);
        // Column count can change later without a new setCustomModel() call (e.g.
        // XFModel_table interleaving presentation columns on setShowPresentation()).
        // Keep the header's per-column filter row in sync with it, or the stale
        // line edits end up positioned against sections that no longer exist.
        connect(pModel, SIGNAL(modelReset()), this, SLOT(onSourceModelReset()), Qt::UniqueConnection);
    } else {
        setModel(pModel);
    }

    adjust();
}

void XTableView::onSourceModelReset()
{
    if (m_pModel) {
        m_pHeaderView->setNumberOfFilters(m_pModel->columnCount());
        m_pHeaderView->updateGeometries();
        m_pHeaderView->update();
        viewport()->update();
    }
}

void XTableView::clear()
{
    m_nCustomFilterGeneration++;
    cancelAsyncOperation();
    m_pSortFilterProxyModel->setSourceModel(nullptr);
    setModel(nullptr);
    deleteOldModel(&m_pModel);
}

void XTableView::deleteOldModel(QAbstractItemModel **ppOldModel)
{
    if (ppOldModel && *ppOldModel) {
        delete (*ppOldModel);

        (*ppOldModel) = nullptr;
    }
}

void XTableView::handleFilter()
{
    QList<QString> listFilters = m_listCurrentFilters;

    qint32 nNumberOfRows = m_pModel->rowCount();
    qint32 nNumberOfFilters = listFilters.count();

    // Clear all hidden states in bulk
    if (m_bIsXmodel) {
        m_pXModel->clearRowHidden();
    }

    // Pre-compute active filter columns to skip empty filters in inner loop
    QVector<qint32> vecActiveColumns;

    for (qint32 j = 0; j < nNumberOfFilters; j++) {
        if (!listFilters.at(j).isEmpty()) {
            vecActiveColumns.append(j);
        }
    }

    qint32 nActiveCount = vecActiveColumns.count();

    for (qint32 i = 0; (i < nNumberOfRows) && (!m_bIsStop); i++) {
        bool bHidden = false;

        for (qint32 k = 0; k < nActiveCount; k++) {
            qint32 nColumn = vecActiveColumns.at(k);
            QModelIndex index = m_pModel->index(i, nColumn);

            if (index.isValid()) {
                QString sValue = m_pModel->data(index).toString();

                if (!sValue.contains(listFilters.at(nColumn), Qt::CaseInsensitive)) {
                    bHidden = true;
                    break;
                }
            }
        }

        // Only set hidden=true; non-hidden rows already cleared by clearRowHidden()
        if (m_bIsXmodel && bHidden) {
            m_pXModel->setRowHidden(i, true);
        }
    }

    if (!m_bIsStop) {
#ifdef QT_DEBUG
        qDebug("XTableView::handleFilter(): Stop at invalid signal");
#endif
        m_pSortFilterProxyModel->blockSignals(true);
        m_pSortFilterProxyModel->invalidate();
        m_pSortFilterProxyModel->blockSignals(false);
        reset();
    } else {
#ifdef QT_DEBUG
        qDebug("XTableView::handleFilter() is stopped");
#endif
    }
}

XSortFilterProxyModel *XTableView::getProxyModel()
{
    return m_pSortFilterProxyModel;
}

void XTableView::setFilterEnabled(qint32 nColumn, bool bFilterEnabled)
{
    m_pHeaderView->setFilterEnabled(nColumn, bFilterEnabled);
}

void XTableView::setColumnFilterString(qint32 nColumn, const QString &sFilter)
{
    if (m_bThreadedEnabled && m_bIsCustomFilter) {
        QList<QString> listFilters = m_listCurrentFilters;

        while (listFilters.count() <= nColumn) {
            listFilters.append(QString());
        }

        listFilters[nColumn] = sFilter;
        m_listCurrentFilters = listFilters;

        startAsyncCustomFilterOperation(listFilters);
        return;
    }

    if (m_bThreadedEnabled && !m_bIsCustomFilter) {
        QList<QString> listFilters = m_pSortFilterProxyModel->getFilters();

        while (listFilters.count() <= nColumn) {
            listFilters.append(QString());
        }

        listFilters[nColumn] = sFilter;
        m_listCurrentFilters = listFilters;

        startAsyncFilterOperation(listFilters);
        return;
    }

    m_pSortFilterProxyModel->setColumnFilter(nColumn, sFilter);
    m_listCurrentFilters = m_pSortFilterProxyModel->getFilters();

    if (m_bIsCustomFilter) {
        m_bIsStop = true;
        m_watcher.waitForFinished();
        m_bIsStop = false;

        handleFilter();
    } else {
        m_pSortFilterProxyModel->invalidate();
    }
}

void XTableView::setThreadedFilterSortEnabled(bool bEnabled)
{
    m_bThreadedEnabled = bEnabled;
}

bool XTableView::isThreadedFilterSortEnabled() const
{
    return m_bThreadedEnabled;
}

void XTableView::setSortingEnabled(bool bEnable)
{
    m_bSortingEnabled = bEnable;

    // XHeaderView emits sortIndicatorChanged() itself and XTableView routes that
    // through the threaded sort path. Keep QTableView's built-in auto-sort
    // disconnected, otherwise the GUI thread performs a synchronous proxy sort
    // before the async cache/progress path can run.
    QTableView::setSortingEnabled(false);

    if (m_pHeaderView) {
        m_pHeaderView->setSectionsClickable(bEnable);
        m_pHeaderView->setSortIndicatorShown(bEnable);
    }
}

bool XTableView::isSortingEnabled() const
{
    return m_bSortingEnabled;
}

void XTableView::sortByColumn(int column, Qt::SortOrder order)
{
    if (!m_bSortingEnabled && (column >= 0)) {
        return;
    }

    if (m_pHeaderView) {
        QSignalBlocker blocker(m_pHeaderView);
        m_pHeaderView->setSortIndicator(column, order);
    }

    onSortChanged(column, order);
}

void XTableView::adjust()
{
    m_pXModel = dynamic_cast<XModel *>(m_pModel);

    if (m_pXModel) {
        qint32 nNumberOfColumns = m_pXModel->columnCount();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            qint32 nSymbolSize = m_pXModel->getColumnSymbolSize(i);

            if (nSymbolSize != -1) {
                qint32 nWidth = XOptions::getControlWidth(this, nSymbolSize);
                setColumnWidth(i, nWidth);
            }
        }
        m_bIsXmodel = true;
        m_bIsCustomFilter = m_pXModel->isCustomFilter();
        m_bIsCustomSort = m_pXModel->isCustomSort();
    } else {
        m_bIsXmodel = false;
        m_bIsCustomFilter = false;
        m_bIsCustomSort = false;
    }
}

void XTableView::onFilterChanged()
{
    m_pFilterTimer->start(300);
}

void XTableView::onFilterApply()
{
#ifdef QT_DEBUG
    // Elapsed time
    QElapsedTimer timer;
    timer.start();
    qDebug("XTableView::onFilterApply(): START");
#endif

    QList<QString> listFilters = m_pHeaderView->getFilters();

    m_listCurrentFilters = listFilters;

    if (m_bIsCustomFilter) {
        if (m_bThreadedEnabled) {
            startAsyncCustomFilterOperation(listFilters);
            return;
        }

        m_pSortFilterProxyModel->setFiltersQuiet(listFilters);
        m_bIsStop = true;
        m_watcher.waitForFinished();
        m_bIsStop = false;

        emit busyChanged(true);
        handleFilter();
        emit busyChanged(false);
    } else {
        if (m_bThreadedEnabled) {
            startAsyncFilterOperation(listFilters);
            return;
        }

        m_pSortFilterProxyModel->setFilters(listFilters);
        emit invalidateSignal();
        // m_pSortFilterProxyModel->invalidate();
    }

#ifdef QT_DEBUG
    qDebug("XTableView::onFilterChanged(): Elapsed time: %lld ms", timer.elapsed());
    // 16266 ms
    // 16342
    qDebug("XTableView::onFilterChanged(): STOP");
#endif
}

void XTableView::onSortChanged(int column, Qt::SortOrder order)
{
    if ((!m_bSortingEnabled && (column >= 0)) || m_bApplyingAsyncSort) {
        return;
    }

    if (m_bThreadedEnabled && !m_bIsCustomSort) {
        startAsyncSortOperation(column, order);
        return;
    }

    if (m_bIsCustomFilter) {
        m_bIsStop = true;
        m_watcher.waitForFinished();
        m_bIsStop = false;
    }

    m_pSortFilterProxyModel->sort(column, order);

    if (m_bIsCustomFilter) {
        handleFilter();
    }
}

void XTableView::horizontalScroll()
{
    m_pHeaderView->adjustPositions();
}

void XTableView::startAsyncCustomFilterOperation(const QList<QString> &listFilters)
{
    m_nCustomFilterGeneration++;
    const qint32 nGeneration = m_nCustomFilterGeneration;

    if (!m_pModel || !m_pXModel) {
        emit busyChanged(false);
        return;
    }

    emit busyChanged(true);

    QVector<qint32> vecActiveColumns;
    QStringList listActiveFilters;

    for (qint32 i = 0; i < listFilters.count(); i++) {
        if (!listFilters.at(i).isEmpty() && (i < m_pModel->columnCount())) {
            vecActiveColumns.append(i);
            listActiveFilters.append(listFilters.at(i));
        }
    }

    if (vecActiveColumns.isEmpty()) {
        m_pSortFilterProxyModel->setFiltersQuiet(listFilters);
        m_pXModel->clearRowHidden();
        m_pSortFilterProxyModel->invalidate();
        reset();
        emit busyChanged(false);
        return;
    }

    const qint32 nNumberOfRows = m_pModel->rowCount();
    QVector<QStringList> listRows;
    listRows.reserve(nNumberOfRows);

    for (qint32 i = 0; i < nNumberOfRows; i++) {
        QStringList listRow;
        listRow.reserve(vecActiveColumns.count());

        for (qint32 j = 0; j < vecActiveColumns.count(); j++) {
            const QModelIndex index = m_pModel->index(i, vecActiveColumns.at(j));
            listRow.append(index.isValid() ? m_pModel->data(index).toString() : QString());
        }

        listRows.append(listRow);
    }

    QFuture<QVector<bool>> future = QtConcurrent::run([listRows, listActiveFilters]() {
        const qint32 nNumberOfRows = listRows.count();
        const qint32 nNumberOfFilters = listActiveFilters.count();
        QVector<bool> vecHidden(nNumberOfRows, false);

        for (qint32 i = 0; i < nNumberOfRows; i++) {
            bool bHidden = false;

            for (qint32 j = 0; j < nNumberOfFilters; j++) {
                if (!listRows.at(i).at(j).contains(listActiveFilters.at(j), Qt::CaseInsensitive)) {
                    bHidden = true;
                    break;
                }
            }

            vecHidden[i] = bHidden;
        }

        return vecHidden;
    });

    QFutureWatcher<QVector<bool>> *pWatcher = new QFutureWatcher<QVector<bool>>(this);

    connect(pWatcher, &QFutureWatcher<QVector<bool>>::finished, this, [this, pWatcher, nGeneration, listFilters]() {
        QVector<bool> vecHidden = pWatcher->result();
        pWatcher->deleteLater();

        if (m_nCustomFilterGeneration != nGeneration) {
            return;
        }

        if (m_pXModel) {
            m_pSortFilterProxyModel->setFiltersQuiet(listFilters);
            m_pXModel->clearRowHidden();

            for (qint32 i = 0; i < vecHidden.count(); i++) {
                if (vecHidden.at(i)) {
                    m_pXModel->setRowHidden(i, true);
                }
            }

            m_pSortFilterProxyModel->invalidate();
            reset();
        }

        emit busyChanged(false);
    });

    pWatcher->setFuture(future);
}

void XTableView::startAsyncFilterOperation(const QList<QString> &listFilters)
{
    cancelAsyncOperation(false);

    m_pendingOperation = OPERATION_FILTER;
    m_listPendingFilters = listFilters;

    XSortFilterProxyModel *pProxy = m_pSortFilterProxyModel;
    m_pAsyncCancelFlag = QSharedPointer<QAtomicInt>::create(0);
    QSharedPointer<QAtomicInt> pCancelFlag = m_pAsyncCancelFlag;

    QFuture<bool> future = QtConcurrent::run([pProxy, listFilters, pCancelFlag]() {
        return pProxy->buildFilterAcceptCache(listFilters, pCancelFlag.data());
    });

    m_pAsyncWatcher = new QFutureWatcher<bool>(this);
    connect(m_pAsyncWatcher, SIGNAL(finished()), this, SLOT(onAsyncOperationFinished()));
    m_pAsyncWatcher->setFuture(future);

    emit busyChanged(true);
}

void XTableView::startAsyncSortOperation(qint32 nColumn, Qt::SortOrder order)
{
    cancelAsyncOperation(false);

    if (nColumn < 0) {
        m_pSortFilterProxyModel->sort(nColumn, order);
        return;
    }

    m_pendingOperation = OPERATION_SORT;
    m_nPendingSortColumn = nColumn;
    m_pendingSortOrder = order;

    XSortFilterProxyModel *pProxy = m_pSortFilterProxyModel;
    m_pAsyncCancelFlag = QSharedPointer<QAtomicInt>::create(0);
    QSharedPointer<QAtomicInt> pCancelFlag = m_pAsyncCancelFlag;

    QFuture<bool> future = QtConcurrent::run([pProxy, nColumn, pCancelFlag]() {
        return pProxy->buildSortCache(nColumn, pCancelFlag.data());
    });

    m_pAsyncWatcher = new QFutureWatcher<bool>(this);
    connect(m_pAsyncWatcher, SIGNAL(finished()), this, SLOT(onAsyncOperationFinished()));
    m_pAsyncWatcher->setFuture(future);

    emit busyChanged(true);
}

void XTableView::cancelAsyncOperation(bool bWait)
{
    if (m_pAsyncCancelFlag) {
        m_pAsyncCancelFlag->storeRelease(1);
    }

    if (m_pAsyncWatcher) {
        disconnect(m_pAsyncWatcher, nullptr, this, nullptr);

        if (bWait) {
            m_pAsyncWatcher->waitForFinished();
            m_pAsyncWatcher->deleteLater();
        } else {
            connect(m_pAsyncWatcher, SIGNAL(finished()), m_pAsyncWatcher, SLOT(deleteLater()));
        }

        m_pAsyncWatcher = nullptr;
    }

    m_pAsyncCancelFlag.clear();
    m_pendingOperation = OPERATION_NONE;
}

void XTableView::onAsyncOperationFinished()
{
    QFutureWatcher<bool> *pWatcher = m_pAsyncWatcher;
    m_pAsyncWatcher = nullptr;

    bool bSuccess = pWatcher->result();
    PENDING_OPERATION op = m_pendingOperation;
    m_pendingOperation = OPERATION_NONE;

    pWatcher->deleteLater();
    m_pAsyncCancelFlag.clear();

    if (bSuccess) {
        if (op == OPERATION_FILTER) {
            m_pSortFilterProxyModel->setFiltersQuiet(m_listPendingFilters);
            m_pSortFilterProxyModel->invalidate();
        } else if (op == OPERATION_SORT) {
            QSignalBlocker blocker(m_pHeaderView);
            m_bApplyingAsyncSort = true;
            m_pSortFilterProxyModel->sort(m_nPendingSortColumn, m_pendingSortOrder);
            m_bApplyingAsyncSort = false;
        }
    }

    emit busyChanged(false);
}
