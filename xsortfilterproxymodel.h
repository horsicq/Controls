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

#ifndef XSORTFILTERPROXYMODEL_H
#define XSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVector>
#include <QAtomicInt>
#include <QMutex>
#include "xmodel.h"

class XSortFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit XSortFilterProxyModel(QObject *pParent = nullptr);
    void setFilters(const QList<QString> &listFilters);
    void setColumnFilter(qint32 nColumn, const QString &sFilter);
    QList<QString> getFilters() const;
    void setSortMethod(qint32 nColumn, XModel::SORT_METHOD sortMethod);
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    void resetModel();

    // Pure read-only computation over sourceModel(); safe to call from a worker thread.
    // pCancelFlag is polled between rows and, if set, aborts early returning false
    // (the corresponding cache is left/marked invalid).
    bool buildSortCache(qint32 nColumn, QAtomicInt *pCancelFlag = nullptr);
    bool buildFilterAcceptCache(const QList<QString> &listFilters, QAtomicInt *pCancelFlag = nullptr);
    void clearFilterAcceptCache();

    // Assigns m_listFilters without triggering invalidateFilter(). For callers (the
    // threaded pipeline) that apply the new filter state themselves once a matching
    // buildFilterAcceptCache() result is ready, instead of paying for a synchronous
    // filter pass twice.
    void setFiltersQuiet(const QList<QString> &listFilters);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    void clearSortCache();

    bool m_bIsXmodel;
    bool m_bIsCustomFilter;
    bool m_bIsCustomSort;
    XModel *m_pXModel;
    QList<QString> m_listFilters;
    QMap<qint32, XModel::SORT_METHOD> m_mapSortMethods;
    bool m_bSortCacheValid;
    qint32 m_nSortCacheColumn;
    XModel::SORT_METHOD m_sortCacheMethod;
    QVector<quint64> m_vecSortCacheHex;
    QVector<QString> m_vecSortCacheStr;

    bool m_bFilterAcceptCacheValid;
    QVector<bool> m_vecFilterAcceptCache;

    // Guards the sort/filter cache members above: buildSortCache()/buildFilterAcceptCache()
    // may run on a worker thread while lessThan()/filterAcceptsRow() run on the GUI thread
    // (e.g. dynamicSortFilter reacting to a source dataChanged mid-build).
    mutable QMutex m_cacheMutex;
};

#endif  // XSORTFILTERPROXYMODEL_H
