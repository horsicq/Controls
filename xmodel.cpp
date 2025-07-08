/* Copyright (c) 2025 hors<horsicq@gmail.com>
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

#include "xmodel.h"

XModel::XModel(QObject *pParent) : QAbstractItemModel(pParent)
{
    g_nRowCount = 0;
    g_nColumnCount = 0;
}

void XModel::setColumnSymbolSize(qint32 nColumn, qint32 nValue)
{
    g_hashColumnSymbolSize[nColumn] = nValue;  // TODO optimize use allocated memory
}

qint32 XModel::getColumnSymbolSize(qint32 nColumn)
{
    return g_hashColumnSymbolSize.value(nColumn, 40);
}

bool XModel::isCustomFilter()
{
    return false;
}

bool XModel::isCustomSort()
{
    return false;
}

void XModel::setRowHidden(qint32 nRow, bool bState)
{
    g_hashRowHidden[nRow] = bState;  // TODO optimize use allocated memory
}

void XModel::setRowPrio(qint32 nRow, quint64 nPrio)
{
    g_hashRowPrio[nRow] = nPrio;
}

bool XModel::isRowHidden(qint32 nRow)
{
    return g_hashRowHidden.value(nRow, false);
}

quint64 XModel::getRowPrio(qint32 nRow)
{
    return g_hashRowPrio.value(nRow, 0);
}

QModelIndex XModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex result;

    if (hasIndex(row, column, parent)) {
        result = createIndex(row, column);
    }

    return result;
}

QModelIndex XModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

void XModel::_setRowCount(qint32 nRowCount)
{
    g_nRowCount = nRowCount;
}

void XModel::_setColumnCount(qint32 nColumnCount)
{
    g_nColumnCount = nColumnCount;
}

int XModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return g_nRowCount;
}

int XModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return g_nColumnCount;
}

void XModel::adjustColumnToContent(qint32 nColumn, bool bHeader)
{
    qint32 nSymbolSize = 0;

    if (bHeader) {
        nSymbolSize = qMax(nSymbolSize, headerData(nColumn, Qt::Horizontal).toString().length());
    }

    qint32 nNumberOfRows = rowCount();

    for (qint32 i = 0; i < nNumberOfRows; i++) {
        QModelIndex index = this->index(i, nColumn);
        QString sData = data(index, Qt::DisplayRole).toString();

        nSymbolSize = qMax(nSymbolSize, sData.length());
    }

    setColumnSymbolSize(nColumn, nSymbolSize);
}

void XModel::adjustColumnsToContent(bool bHeader)
{
    qint32 nNumberOfColumns = columnCount();

    for (qint32 i = 0; i < nNumberOfColumns; i++) {
        adjustColumnToContent(i, bHeader);
    }
}

XModel::SORT_METHOD XModel::getSortMethod(qint32 nColumn)
{
    Q_UNUSED(nColumn)

    return SORT_METHOD_DEFAULT;
}
