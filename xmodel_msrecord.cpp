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

XModel_MSRecord::XModel_MSRecord(QIODevice *pDevice, const XBinary::_MEMORY_MAP &memoryMap, QVector<XBinary::MS_RECORD> *pListRecods, XBinary::MS_RECORD_TYPE msRecordType, QObject *pParent): QAbstractItemModel(pParent)
{
    g_pDevice = pDevice;
    g_memoryMap = memoryMap;
    g_pListRecords = pListRecods;
    g_msRecordType = msRecordType;

    g_nRowCount = pListRecods->count();
    g_nColumnCount = __COLUMN_SIZE;

    {
        XADDR nMaxAddress = 0;
        qint64 nMaxOffset = 0;
        qint64 nMaxSize = 0;

        qint32 nNumberOfRecords = pListRecods->count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            nMaxAddress = qMax(nMaxAddress, pListRecods->at(i).nAddress);
            nMaxOffset = qMax(nMaxOffset, pListRecods->at(i).nOffset);
            nMaxSize = qMax(nMaxSize, pListRecods->at(i).nSize);
        }

        g_modeAddress = XBinary::getWidthModeFromSize(nMaxAddress);
        g_modeOffset = XBinary::getWidthModeFromSize(nMaxOffset);
        g_modeSize = XBinary::getWidthModeFromSize(nMaxSize);
    }

    g_nColumnWidths[COLUMN_NUMBER] = QString::number(pListRecods->count()).length();
    g_nColumnWidths[COLUMN_OFFSET]=XBinary::getByteSizeFromWidthMode(g_modeOffset) * 2;
    g_nColumnWidths[COLUMN_ADDRESS]= XBinary::getByteSizeFromWidthMode(g_modeAddress) * 2;
    g_nColumnWidths[COLUMN_REGION]=1;
    g_nColumnWidths[COLUMN_SIZE]= XBinary::getByteSizeFromWidthMode(g_modeSize) * 2;
    g_nColumnWidths[COLUMN_TYPE]=1;
    g_nColumnWidths[COLUMN_VALUE]=100;

    qint32 nNumberOfRegions = memoryMap.listRecords.count();

    for (qint32 i = 0; i < nNumberOfRegions; i++) {
        g_nColumnWidths[COLUMN_REGION] = qMax(g_nColumnWidths[COLUMN_REGION], memoryMap.listRecords.at(i).sName.length());
        g_nColumnWidths[COLUMN_REGION] = qMin(g_nColumnWidths[COLUMN_REGION], 50);
    }
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

QVariant XModel_MSRecord::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (index.isValid()) {
        qint32 nRow = index.row();

        if (nRow >= 0) {
            qint32 nColumn = index.column();

            if (nRole == Qt::DisplayRole) {
                if (nColumn == COLUMN_NUMBER) {
                    result = QString::number(nRow);
                } else if (nColumn == COLUMN_OFFSET) {
                    result = XBinary::valueToHex(g_pListRecords->at(nRow).nOffset);
                } else if (nColumn == COLUMN_ADDRESS) {
                    result = XBinary::valueToHex(g_pListRecords->at(nRow).nAddress);
                } else if (nColumn == COLUMN_REGION) {
                    if (g_pListRecords->at(nRow).nRegionIndex >= 0) {
                        result = g_memoryMap.listRecords.at(g_pListRecords->at(nRow).nRegionIndex).sName;
                    }
                } else if (nColumn == COLUMN_SIZE) {
                    result = XBinary::valueToHex(g_pListRecords->at(nRow).nSize);
                } else if (nColumn == COLUMN_TYPE) {
                    result = XBinary::msRecordTypeIdToString(g_pListRecords->at(nRow).recordType);
                } else if (nColumn == COLUMN_VALUE) {
                    XBinary binary(g_pDevice);
                    result = binary.read_msRecordValue(g_pListRecords->at(nRow).recordType, g_pListRecords->at(nRow).nOffset, g_pListRecords->at(nRow).nSize);
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                if ((nColumn == COLUMN_NUMBER) || (nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_ADDRESS) || (nColumn == COLUMN_SIZE)) {
                    result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignRight;
                } else {
                    result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignLeft;
                }
            } else if (nRole == Qt::UserRole + USERROLE_ADDRESS) {
                result = g_pListRecords->at(nRow).nAddress;
            } else if (nRole == Qt::UserRole + USERROLE_OFFSET) {
                result = g_pListRecords->at(nRow).nOffset;
            } else if (nRole == Qt::UserRole + USERROLE_SIZE) {
                result = g_pListRecords->at(nRow).nSize;
            } else if (nRole == Qt::UserRole + USERROLE_TYPE) {
                result = g_pListRecords->at(nRow).recordType;
            }
        }
    }

    return result;
}

QVariant XModel_MSRecord::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (nRole == Qt::DisplayRole) {
            if (nSection == COLUMN_NUMBER) {
                result = "#";
            } else if (nSection == COLUMN_OFFSET) {
                result = tr("Offset");
            } else if (nSection == COLUMN_ADDRESS) {
                result = tr("Address");
            } else if (nSection == COLUMN_REGION) {
                result = tr("Region");
            } else if (nSection == COLUMN_SIZE) {
                result = tr("Size");
            } else if (nSection == COLUMN_TYPE) {
                result = tr("Type");
            } else if (nSection == COLUMN_VALUE) {
                if (g_msRecordType == XBinary::MS_RECORD_TYPE_STRING) {
                    result = tr("String");
                } else {
                    result = tr("Value");
                }
            }
        } else if (nRole == Qt::TextAlignmentRole) {
            if ((nSection == COLUMN_NUMBER) || (nSection == COLUMN_OFFSET) || (nSection == COLUMN_ADDRESS) || (nSection == COLUMN_SIZE)) {
                result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignRight;
            } else {
                result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignLeft;
            }
        }
    }

    return result;
}

qint32 XModel_MSRecord::getColumnSymbolSize(qint32 nColumn)
{
    return g_nColumnWidths[nColumn];
}

