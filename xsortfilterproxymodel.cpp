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

void XSortFilterProxyModel::setFilters(const QList<QString> &listFilters)
{
    g_listFilters = listFilters;
    invalidateFilter();
}

void XSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    g_listFilters.clear();

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool XSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool bResult = true;

    qint32 nCount = g_listFilters.count();

    for (qint32 i = 0; i < nCount; i++) {
        if (g_listFilters.at(i) != "") {
            QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);

            if (index.isValid()) {
                QString sValue = index.data().toString();

                if (!sValue.contains(g_listFilters.at(i), Qt::CaseInsensitive)) {
                    bResult = false;
                    break;
                }
            }
        }
    }

    return bResult;
}

// bool XSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
// {

// }
