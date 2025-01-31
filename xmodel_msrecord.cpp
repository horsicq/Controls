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

#include "xmodel_msrecord.h"

XModel_MSRecord::XModel_MSRecord(QVector<XBinary::MS_RECORD> *pListRecods, QObject *pParent): QAbstractItemModel(pParent)
{
    g_pListRecords = pListRecods;

    g_nRowCount = pListRecods->count();
    g_nColumnCount = 6;
}

QModelIndex XModel_MSRecord::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex result;

    if(hasIndex(row, column, parent))
    {
        result=createIndex(row, column);
    }

    return result;
}

QModelIndex XModel_MSRecord::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

int XModel_MSRecord::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return g_nRowCount;
}

int XModel_MSRecord::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return g_nColumnCount;
}

QVariant XModel_MSRecord::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

