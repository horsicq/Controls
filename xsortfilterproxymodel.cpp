/* Copyright (c) 2024 hors<horsicq@gmail.com>
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

XSortFilterProxyModel::XSortFilterProxyModel(QObject *pParent) : QSortFilterProxyModel(pParent)
{
}

void XSortFilterProxyModel::setFilter(qint32 nColumn, const QString &sFilter)
{
    g_mapFilters.insert(nColumn, sFilter);

    invalidateFilter();
}

void XSortFilterProxyModel::setSortMethod(qint32 nColumn, XModel::SORT_METHOD sortMethod)
{
    g_mapSortMethods.insert(nColumn, sortMethod);
}

void XSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    g_mapFilters.clear();

    XModel *pModel = dynamic_cast<XModel *>(sourceModel);

    if (pModel) {
        qint32 nNumberOfColumns = pModel->columnCount();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            XModel::SORT_METHOD sortMethod = pModel->getSortMethod(i);

            setSortMethod(i, sortMethod);
        }
    }

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

QVariant XSortFilterProxyModel::data(const QModelIndex &index, int nRole) const
{
    return QSortFilterProxyModel::data(index, nRole);
}

bool XSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool bResult = true;

    QList<qint32> listKeys = g_mapFilters.keys();

    qint32 nCount = listKeys.count();

    for (qint32 i = 0; i < nCount; i++) {
        QString sFilter = g_mapFilters.value(listKeys.at(i));
        if (sFilter != "") {
            QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);

            if (index.isValid()) {
                QString sValue = index.data().toString();

                if (!sValue.contains(sFilter, Qt::CaseInsensitive)) {
                    bResult = false;
                    break;
                }
            }
        }
    }

    return bResult;
}

bool XSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool bResult = false;

    qint32 nColumn = left.column();

    XModel::SORT_METHOD sortMethod = g_mapSortMethods.value(nColumn, XModel::SORT_METHOD_DEFAULT);

    if (sortMethod == XModel::SORT_METHOD_HEX) {
        QString sLeft = left.data().toString();
        QString sRight = right.data().toString();

        sLeft = sLeft.remove(" ");
        sRight = sRight.remove(" ");

        bResult = sLeft.toULongLong(0, 16) < sRight.toULongLong(0, 16);
    } else {
        bResult = QSortFilterProxyModel::lessThan(left, right);
    }

    return bResult;
}
