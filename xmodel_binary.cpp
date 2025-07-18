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

XModel_Binary::XModel_Binary(const XBinary::DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<XBinary::DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles, QObject *pParent) : XModel(pParent)
{
    g_dataRecordsOptions = dataRecordsOptions;
    g_pListDataRecords = pListDataRecords;
    g_pListTitles = pListTitles;
    g_pListComments = nullptr;

    if (dataRecordsOptions.dataHeader.dhMode == XBinary::DHMODE_HEADER) {
        _setRowCount(dataRecordsOptions.dataHeader.listRecords.count());
        _setColumnCount(__HEADER_COLUMN_SIZE);

        setColumnName(HEADER_COLUMN_NAME, tr("Name"));
        setColumnName(HEADER_COLUMN_OFFSET, tr("Offset"));
        setColumnName(HEADER_COLUMN_SIZE, tr("Size"));
        setColumnName(HEADER_COLUMN_TYPE, tr("Type"));
        setColumnName(HEADER_COLUMN_VALUE, tr("Value"));
        setColumnName(HEADER_COLUMN_INFO, tr(""));
        setColumnName(HEADER_COLUMN_COMMENT, tr("Comment"));
        setColumnAlignment(HEADER_COLUMN_NAME, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(HEADER_COLUMN_OFFSET, Qt::AlignVCenter | Qt::AlignRight);
        setColumnAlignment(HEADER_COLUMN_SIZE, Qt::AlignVCenter | Qt::AlignRight);
        setColumnAlignment(HEADER_COLUMN_TYPE, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(HEADER_COLUMN_VALUE, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(HEADER_COLUMN_INFO, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(HEADER_COLUMN_COMMENT, Qt::AlignVCenter | Qt::AlignLeft);
    } else if (dataRecordsOptions.dataHeader.dhMode == XBinary::DHMODE_TABLE) {
        _setColumnCount(pListTitles->count());
        _setRowCount(pListDataRecords->count());

        qint32 nNumberOfColumns = g_pListTitles->count();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            qint32 flag = Qt::AlignVCenter | Qt::AlignLeft;

            XBinary::VT valType = dataRecordsOptions.dataHeader.listRecords.at(i).valType;

            if (XBinary::isIntegerType(valType)) {
                flag = Qt::AlignVCenter | Qt::AlignRight;
            }

            setColumnAlignment(i, flag);
            setColumnName(i, g_pListTitles->at(i));
        }
    }
}

QVariant XModel_Binary::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (index.isValid()) {
        qint32 nRow = index.row();

        if (nRow >= 0) {
            qint32 nColumn = index.column();

            if (nRole == Qt::DisplayRole) {
                if (g_dataRecordsOptions.dataHeader.dhMode == XBinary::DHMODE_HEADER) {
                    if (nColumn == HEADER_COLUMN_NAME) {
                        result = g_dataRecordsOptions.dataHeader.listRecords.at(nRow).sName;
                    } else if (nColumn == HEADER_COLUMN_OFFSET) {
                        result = QString::number(g_dataRecordsOptions.dataHeader.listRecords.at(nRow).nRelOffset, 16);
                    } else if (nColumn == HEADER_COLUMN_SIZE) {
                        result = QString::number(g_dataRecordsOptions.dataHeader.listRecords.at(nRow).nSize, 16);
                    } else if (nColumn == HEADER_COLUMN_TYPE) {
                        result = XBinary::valueTypeToString(g_dataRecordsOptions.dataHeader.listRecords.at(nRow).valType, g_dataRecordsOptions.dataHeader.listRecords.at(nRow).nSize);
                    } else if (nColumn == HEADER_COLUMN_VALUE) {
                        result = XBinary::getValueString(g_pListDataRecords->at(0).listValues.at(nRow), g_dataRecordsOptions.dataHeader.listRecords.at(nRow).valType, true);
                    } else if (nColumn == HEADER_COLUMN_INFO) {
                         // TODO
                    } else if (nColumn == HEADER_COLUMN_COMMENT) {
                        if (g_pListComments) {
                            if (nRow < g_pListComments->count()) {
                                result = g_pListComments->at(nRow);
                            }
                        }
                    }
                } else if (g_dataRecordsOptions.dataHeader.dhMode == XBinary::DHMODE_TABLE) {
                    result = XBinary::getValueString(g_pListDataRecords->at(nRow).listValues.at(nColumn), g_dataRecordsOptions.dataHeader.listRecords.at(nColumn).valType, true);
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            }
        }
    }

    return result;
}

QVariant XModel_Binary::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (nRole == Qt::DisplayRole) {
            result = getColumnName(nSection);
        } else if (nRole == Qt::TextAlignmentRole) {
            result = getColumnAlignment(nSection);
        }
    }

    return result;
}

void XModel_Binary::setComments(QList<QString> *pListComments)
{
    g_pListComments = pListComments;
}
