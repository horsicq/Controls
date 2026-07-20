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

#include "xsortfilterproxymodel.h"

namespace {
bool isNumericVariant(const QVariant &value)
{
    switch (value.userType()) {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Double:
            return true;
        default:
            return false;
    }
}

bool variantLessThan(const QVariant &left, const QVariant &right)
{
    if (isNumericVariant(left) && isNumericVariant(right)) {
        return left.toDouble() < right.toDouble();
    }

    return left.toString() < right.toString();
}
}  // namespace

XSortFilterProxyModel::XSortFilterProxyModel(QObject *pParent) : QSortFilterProxyModel(pParent)
{
    m_bIsXmodel = false;
    m_bIsCustomFilter = false;
    m_bIsCustomSort = false;
    m_pXModel = nullptr;
    m_bSortCacheValid = false;
    m_nSortCacheColumn = -1;
    m_sortCacheMethod = XModel::SORT_METHOD_DEFAULT;
    m_bFilterAcceptCacheValid = false;
}

void XSortFilterProxyModel::setFilters(const QList<QString> &listFilters)
{
    m_listFilters = listFilters;
    clearFilterAcceptCache();
    invalidateFilter();
}

void XSortFilterProxyModel::setColumnFilter(qint32 nColumn, const QString &sFilter)
{
    while (m_listFilters.count() <= nColumn) {
        m_listFilters.append(QString());
    }

    m_listFilters[nColumn] = sFilter;
    clearFilterAcceptCache();
    invalidateFilter();
}

void XSortFilterProxyModel::setFiltersQuiet(const QList<QString> &listFilters)
{
    m_listFilters = listFilters;
}

QList<QString> XSortFilterProxyModel::getFilters() const
{
    return m_listFilters;
}

void XSortFilterProxyModel::setSortMethod(qint32 nColumn, XModel::SORT_METHOD sortMethod)
{
    m_mapSortMethods.insert(nColumn, sortMethod);
    clearSortCache();
}

void XSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_listFilters.clear();
    m_mapSortMethods.clear();
    clearSortCache();
    clearFilterAcceptCache();

    m_pXModel = dynamic_cast<XModel *>(sourceModel);

    if (m_pXModel) {
        qint32 nNumberOfColumns = m_pXModel->columnCount();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            XModel::SORT_METHOD sortMethod = m_pXModel->getSortMethod(i);

            setSortMethod(i, sortMethod);
        }

        m_bIsXmodel = true;
        m_bIsCustomFilter = m_pXModel->isCustomFilter();
        m_bIsCustomSort = m_pXModel->isCustomSort();
    } else {
        m_bIsXmodel = false;
        m_bIsCustomFilter = false;
        m_bIsCustomSort = false;
    }

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

QVariant XSortFilterProxyModel::data(const QModelIndex &index, int nRole) const
{
    return QSortFilterProxyModel::data(index, nRole);
}

void XSortFilterProxyModel::sort(int column, Qt::SortOrder order)
{
    if (column < 0) {
        QSortFilterProxyModel::sort(column, order);
        clearSortCache();
        return;
    }

    if (m_bIsCustomSort && m_pXModel) {
        m_pXModel->sortByColumn(column, order);
    } else {
        if (!(m_bSortCacheValid && (m_nSortCacheColumn == column))) {
            buildSortCache(column);
        }
        QSortFilterProxyModel::sort(column, order);
        clearSortCache();
    }
}

void XSortFilterProxyModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

bool XSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool bResult = true;

    if (m_bIsCustomFilter) {
        bResult = !(m_pXModel->isRowHidden(sourceRow));
        return bResult;
    }

    bool bCacheValid = false;
    bool bCachedResult = true;

    {
        QMutexLocker locker(&m_cacheMutex);
        bCacheValid = m_bFilterAcceptCacheValid;
        if (bCacheValid) {
            bCachedResult = ((sourceRow >= 0) && (sourceRow < m_vecFilterAcceptCache.count())) ? m_vecFilterAcceptCache.at(sourceRow) : true;
        }
    }

    if (bCacheValid) {
        bResult = bCachedResult;
    } else {
        qint32 nCount = m_listFilters.count();

        for (qint32 i = 0; i < nCount; i++) {
            QString sFilter = m_listFilters.at(i);
            if (sFilter != "") {
                QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);

                if (index.isValid()) {
                    QString sValue = sourceModel()->data(index).toString();

                    if (!sValue.contains(sFilter, Qt::CaseInsensitive)) {
                        bResult = false;
                        break;
                    }
                }
            }
        }
    }

    return bResult;
}

bool XSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool bResult = false;

    if (m_bIsCustomSort) {
        bResult = (left.row() < right.row());
        return bResult;
    }

    bool bCacheValid = false;

    {
        QMutexLocker locker(&m_cacheMutex);
        bCacheValid = m_bSortCacheValid;

        if (bCacheValid) {
            qint32 nLeftRow = left.row();
            qint32 nRightRow = right.row();

            if (m_sortCacheMethod == XModel::SORT_METHOD_HEX) {
                bResult = m_vecSortCacheHex.at(nLeftRow) < m_vecSortCacheHex.at(nRightRow);
            } else {
                bResult = variantLessThan(m_vecSortCacheVariants.at(nLeftRow), m_vecSortCacheVariants.at(nRightRow));
            }
        }
    }

    if (!bCacheValid) {
        qint32 nColumn = left.column();

        XModel::SORT_METHOD sortMethod = m_mapSortMethods.value(nColumn, XModel::SORT_METHOD_DEFAULT);

        if (sortMethod == XModel::SORT_METHOD_HEX) {
            if (m_bIsXmodel && m_pXModel && m_pXModel->hasSortKeyHex()) {
                bResult = m_pXModel->getSortKeyHex(left.row(), nColumn) < m_pXModel->getSortKeyHex(right.row(), nColumn);
            } else {
                QString sLeft = left.data().toString();
                QString sRight = right.data().toString();

                sLeft = sLeft.remove(" ");
                sRight = sRight.remove(" ");

                bResult = sLeft.toULongLong(0, 16) < sRight.toULongLong(0, 16);
            }
        } else {
            bResult = QSortFilterProxyModel::lessThan(left, right);
        }
    }

    return bResult;
}

bool XSortFilterProxyModel::buildSortCache(qint32 nColumn, QAtomicInt *pCancelFlag)
{
    QAbstractItemModel *pSource = sourceModel();

    if (!pSource) {
        QMutexLocker locker(&m_cacheMutex);
        m_bSortCacheValid = false;
        return false;
    }

    qint32 nRowCount = pSource->rowCount();
    XModel::SORT_METHOD sortMethod = m_mapSortMethods.value(nColumn, XModel::SORT_METHOD_DEFAULT);

    // Computed into locals first — only reads sourceModel(), no shared-state writes,
    // so this loop is safe to run concurrently with GUI-thread reads of the (still
    // previous, still valid) cache below.
    QVector<quint64> vecHex;
    QVector<QVariant> vecVariants;

    if (sortMethod == XModel::SORT_METHOD_HEX) {
        vecHex.resize(nRowCount);

        if (m_bIsXmodel && m_pXModel && m_pXModel->hasSortKeyHex()) {
            for (qint32 i = 0; i < nRowCount; i++) {
                if (pCancelFlag && pCancelFlag->loadAcquire()) {
                    return false;
                }
                vecHex[i] = m_pXModel->getSortKeyHex(i, nColumn);
            }
        } else {
            for (qint32 i = 0; i < nRowCount; i++) {
                if (pCancelFlag && pCancelFlag->loadAcquire()) {
                    return false;
                }
                QModelIndex idx = pSource->index(i, nColumn);
                QString sVal = pSource->data(idx).toString().remove(" ");
                vecHex[i] = sVal.toULongLong(nullptr, 16);
            }
        }
    } else {
        vecVariants.resize(nRowCount);

        for (qint32 i = 0; i < nRowCount; i++) {
            if (pCancelFlag && pCancelFlag->loadAcquire()) {
                return false;
            }
            QModelIndex idx = pSource->index(i, nColumn);
            vecVariants[i] = pSource->data(idx);
        }
    }

    if (pCancelFlag && pCancelFlag->loadAcquire()) {
        return false;
    }

    QMutexLocker locker(&m_cacheMutex);

    if (sortMethod == XModel::SORT_METHOD_HEX) {
        m_vecSortCacheHex = vecHex;
        m_vecSortCacheVariants.clear();
    } else {
        m_vecSortCacheVariants = vecVariants;
        m_vecSortCacheHex.clear();
    }

    m_sortCacheMethod = sortMethod;
    m_nSortCacheColumn = nColumn;
    m_bSortCacheValid = true;

    return true;
}

void XSortFilterProxyModel::clearSortCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_vecSortCacheHex.clear();
    m_vecSortCacheVariants.clear();
    m_bSortCacheValid = false;
    m_nSortCacheColumn = -1;
}

bool XSortFilterProxyModel::buildFilterAcceptCache(const QList<QString> &listFilters, QAtomicInt *pCancelFlag)
{
    QAbstractItemModel *pSource = sourceModel();

    if (!pSource) {
        QMutexLocker locker(&m_cacheMutex);
        m_bFilterAcceptCacheValid = false;
        return false;
    }

    qint32 nRowCount = pSource->rowCount();
    qint32 nNumberOfFilters = listFilters.count();

    QVector<qint32> vecActiveColumns;

    for (qint32 j = 0; j < nNumberOfFilters; j++) {
        if (!listFilters.at(j).isEmpty()) {
            vecActiveColumns.append(j);
        }
    }

    qint32 nActiveCount = vecActiveColumns.count();
    QVector<bool> vecResult(nRowCount, true);

    for (qint32 i = 0; i < nRowCount; i++) {
        if (pCancelFlag && pCancelFlag->loadAcquire()) {
            return false;
        }

        bool bAccepted = true;

        for (qint32 k = 0; k < nActiveCount; k++) {
            qint32 nColumn = vecActiveColumns.at(k);
            QModelIndex index = pSource->index(i, nColumn);

            if (index.isValid()) {
                QString sValue = pSource->data(index).toString();

                if (!sValue.contains(listFilters.at(nColumn), Qt::CaseInsensitive)) {
                    bAccepted = false;
                    break;
                }
            }
        }

        vecResult[i] = bAccepted;
    }

    if (pCancelFlag && pCancelFlag->loadAcquire()) {
        return false;
    }

    QMutexLocker locker(&m_cacheMutex);
    m_vecFilterAcceptCache = vecResult;
    m_bFilterAcceptCacheValid = true;

    return true;
}

void XSortFilterProxyModel::clearFilterAcceptCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_vecFilterAcceptCache.clear();
    m_bFilterAcceptCacheValid = false;
}
