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
#include "xmodel_binary.h"

XModel_Binary::XModel_Binary(const XBinary::DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<QVariant> *pListValues, QList<QString> *pListTitles, QObject *pParent) : XModel(pParent)
{
    g_dataRecordsOptions = dataRecordsOptions;
    g_pListValues = pListValues;
    g_pListTitles = pListTitles;

    _setColumnCount(pListTitles->count());
    _setRowCount(dataRecordsOptions.dataHeader.nCount);
}

QVariant XModel_Binary::data(const QModelIndex &index, int nRole) const
{
    QVariant varResult;

    return varResult;
}

QVariant XModel_Binary::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (nRole == Qt::DisplayRole) {
            if (nSection < g_pListTitles->count()) {
                result = g_pListTitles->at(nSection);
            }
        }
    }

    return result;
}
