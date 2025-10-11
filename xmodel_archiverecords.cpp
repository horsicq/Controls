/* Copyright (c) 2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to do so, subject to the
 * following conditions:
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
#include "xmodel_archiverecords.h"

XModel_ArchiveRecords::XModel_ArchiveRecords(QList<XBinary::ARCHIVERECORD> *pListArchiveRecords, QObject *pParent) : XModel(pParent)
{
    m_pListArchiveRecords = pListArchiveRecords;
    if (pListArchiveRecords) {
        _setRowCount(pListArchiveRecords->count());
    } else {
        _setRowCount(0);
    }
    _setColumnCount(__COLUMN_COUNT);
    _initColumns();
}

void XModel_ArchiveRecords::_initColumns()
{
    setColumnName(COLUMN_NAME, QObject::tr("Name"));
    setColumnName(COLUMN_STREAMOFFSET, QObject::tr("Stream offset"));
    setColumnName(COLUMN_STREAMSIZE, QObject::tr("Stream size"));
    setColumnName(COLUMN_DECOMPRESSEDOFFSET, QObject::tr("Decompressed offset"));
    setColumnName(COLUMN_DECOMPRESSEDSIZE, QObject::tr("Decompressed size"));
    setColumnName(COLUMN_COMPRESSMETHOD, QObject::tr("Compress"));
    setColumnName(COLUMN_CRC, QObject::tr("CRC"));
    setColumnName(COLUMN_DATETIME, QObject::tr("Date"));

    setColumnAlignment(COLUMN_NAME, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_STREAMOFFSET, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_STREAMSIZE, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_DECOMPRESSEDOFFSET, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_DECOMPRESSEDSIZE, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_COMPRESSMETHOD, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_CRC, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_DATETIME, Qt::AlignVCenter | Qt::AlignLeft);

    setColumnSymbolSize(COLUMN_NAME, 20);
    setColumnSymbolSize(COLUMN_STREAMOFFSET, 16);
    setColumnSymbolSize(COLUMN_STREAMSIZE, 16);
    setColumnSymbolSize(COLUMN_DECOMPRESSEDOFFSET, 16);
    setColumnSymbolSize(COLUMN_DECOMPRESSEDSIZE, 16);
    setColumnSymbolSize(COLUMN_COMPRESSMETHOD, 10);
    setColumnSymbolSize(COLUMN_CRC, 8);
    setColumnSymbolSize(COLUMN_DATETIME, 19);
}

QVariant XModel_ArchiveRecords::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!m_pListArchiveRecords) {
        return result;
    }

    if (index.isValid()) {
        qint32 nRow = index.row();
        if (nRow >= 0 && nRow < m_pListArchiveRecords->count()) {
            qint32 nColumn = index.column();
            const XBinary::ARCHIVERECORD &rec = m_pListArchiveRecords->at(nRow);
            if (nRole == Qt::DisplayRole) {
                if (nColumn == COLUMN_NAME) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_ORIGINALNAME)) {
                        result = rec.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
                    }
                } else if (nColumn == COLUMN_STREAMOFFSET) {
                    result = QString::number(rec.nStreamOffset, 16);
                } else if (nColumn == COLUMN_STREAMSIZE) {
                    result = QString::number(rec.nStreamSize, 16);
                } else if (nColumn == COLUMN_DECOMPRESSEDOFFSET) {
                    result = QString::number(rec.nDecompressedOffset, 16);
                } else if (nColumn == COLUMN_DECOMPRESSEDSIZE) {
                    result = QString::number(rec.nDecompressedSize, 16);
                } else if (nColumn == COLUMN_COMPRESSMETHOD) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_COMPRESSMETHOD)) {
                        XBinary::COMPRESS_METHOD cm = (XBinary::COMPRESS_METHOD)rec.mapProperties.value(XBinary::FPART_PROP_COMPRESSMETHOD).toInt();
                        result = XBinary::compressMethodToString(cm);
                    }
                } else if (nColumn == COLUMN_CRC) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_CRC_VALUE)) {
                        quint32 nCRC = rec.mapProperties.value(XBinary::FPART_PROP_CRC_VALUE).toUInt();
                        result = QString::number(nCRC, 16);
                    }
                } else if (nColumn == COLUMN_DATETIME) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_DATETIME)) {
                        QDateTime dt = rec.mapProperties.value(XBinary::FPART_PROP_DATETIME).toDateTime();
                        if (dt.isValid()) {
                            result = dt.toString("yyyy-MM-dd HH:mm:ss");
                        }
                    }
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            } else if (nRole >= Qt::UserRole) {
                if (nRole == (Qt::UserRole + XModel::USERROLE_ORIGINDEX)) {
                    result = nRow;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_OFFSET)) {
                    result = rec.nStreamOffset;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_SIZE)) {
                    result = rec.nStreamSize;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING1)) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_ORIGINALNAME)) {
                        result = rec.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
                    }
                }
            }
        }
    }

    return result;
}

QVariant XModel_ArchiveRecords::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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
