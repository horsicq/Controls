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

XModel_MSRecord::XModel_MSRecord(QIODevice *pDevice, const XBinary::_MEMORY_MAP &memoryMap, QVector<XBinary::MS_RECORD> *pListRecods, XBinary::VT valueType,
                                 QObject *pParent)
    : XModel(pParent)
{
    g_pDevice = pDevice;
    g_memoryMap = memoryMap;
    g_pListRecords = pListRecods;
    g_pListSignatureRecords = nullptr;

    g_endian = XBinary::ENDIAN_LITTLE;
    g_valueType = valueType;

    g_nRowCount = pListRecods->count();
    g_nColumnCount = __COLUMN_SIZE;

    g_modeAddress = XBinary::getWidthModeFromSize(memoryMap.nModuleAddress + memoryMap.nImageSize);
    g_modeOffset = XBinary::getWidthModeFromSize(memoryMap.nBinarySize);

    g_nColumnWidths[COLUMN_NUMBER] = QString::number(g_nRowCount).length();
    g_nColumnWidths[COLUMN_OFFSET] = XBinary::getByteSizeFromWidthMode(g_modeOffset) * 2;
    g_nColumnWidths[COLUMN_ADDRESS] = XBinary::getByteSizeFromWidthMode(g_modeAddress) * 2;
    g_nColumnWidths[COLUMN_REGION] = 1;
    g_nColumnWidths[COLUMN_SIZE] = 4;
    g_nColumnWidths[COLUMN_INFO] = 8;
    g_nColumnWidths[COLUMN_VALUE] = 100;

    qint32 nNumberOfRegions = memoryMap.listRecords.count();

    for (qint32 i = 0; i < nNumberOfRegions; i++) {
        g_nColumnWidths[COLUMN_REGION] = qMax(g_nColumnWidths[COLUMN_REGION], memoryMap.listRecords.at(i).sName.length());
        g_nColumnWidths[COLUMN_REGION] = qMin(g_nColumnWidths[COLUMN_REGION], 50);
    }
}

void XModel_MSRecord::setValue(XBinary::ENDIAN endian, XBinary::VT valueType, QVariant varValue)
{
    g_endian = endian;
    g_valueType = valueType;
    g_varValue = varValue;

    g_sValue = XBinary::getValueString(varValue, valueType);
}

void XModel_MSRecord::setSignaturesList(QList<XBinary::SIGNATUREDB_RECORD> *pListSignatureRecords)
{
    g_pListSignatureRecords = pListSignatureRecords;
}

QModelIndex XModel_MSRecord::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex result;

    if (hasIndex(row, column, parent)) {
        result = createIndex(row, column);
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
                    result = nRow;
                } else if (nColumn == COLUMN_OFFSET) {
                    qint16 nRegionIndex = g_pListRecords->at(nRow).nRegionIndex;
                    if (nRegionIndex != -1) {
                        if (g_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                            result = XBinary::valueToHex(g_modeOffset, g_memoryMap.listRecords.at(nRegionIndex).nOffset + g_pListRecords->at(nRow).nRelOffset);
                        }
                    } else {
                        result = XBinary::valueToHex(g_modeOffset, g_pListRecords->at(nRow).nRelOffset);
                    }
                } else if (nColumn == COLUMN_ADDRESS) {
                    qint16 nRegionIndex = g_pListRecords->at(nRow).nRegionIndex;
                    if (nRegionIndex != -1) {
                        if (g_memoryMap.listRecords.at(nRegionIndex).nAddress != -1) {
                            result = XBinary::valueToHex(g_modeAddress, g_memoryMap.listRecords.at(nRegionIndex).nAddress + g_pListRecords->at(nRow).nRelOffset);
                        }
                    }
                } else if (nColumn == COLUMN_REGION) {
                    if (g_pListRecords->at(nRow).nRegionIndex >= 0) {
                        result = g_memoryMap.listRecords.at(g_pListRecords->at(nRow).nRegionIndex).sName;
                    }
                } else if (nColumn == COLUMN_SIZE) {
                    result = QString::number(g_pListRecords->at(nRow).nSize, 16);
                } else if (nColumn == COLUMN_INFO) {
                    if (g_valueType != XBinary::VT_SIGNATURE) {
                        result = XBinary::valueTypeToString((XBinary::VT)(g_pListRecords->at(nRow).nValueType));
                    } else {
                        if (nColumn == COLUMN_INFO) {
                            if (g_valueType == XBinary::VT_SIGNATURE) {
                                if (g_pListSignatureRecords && (g_pListSignatureRecords->count() > g_pListRecords->at(nRow).nInfo)) {
                                    if (g_pListSignatureRecords->at(g_pListRecords->at(nRow).nInfo).sPatch != "") {
                                        result = tr("Patch");
                                    }
                                }
                            }
                        }
                    }
                } else if (nColumn == COLUMN_VALUE) {
                    if ((g_valueType == XBinary::VT_STRING) || (g_valueType == XBinary::VT_ANSISTRING_I) || (g_valueType == XBinary::VT_UNICODESTRING_I) ||
                        (g_valueType == XBinary::VT_UTF8STRING_I)) {
                        XBinary binary(g_pDevice);
                        XBinary::VT valueType = g_valueType;
                        if (g_valueType == XBinary::VT_STRING) {
                            valueType = (XBinary::VT)(g_pListRecords->at(nRow).nValueType);
                        }
                        qint16 nRegionIndex = g_pListRecords->at(nRow).nRegionIndex;

                        if (nRegionIndex != -1) {
                            if (g_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                                qint64 _nOffset = g_memoryMap.listRecords.at(nRegionIndex).nOffset + g_pListRecords->at(nRow).nRelOffset;
                                result = binary.read_valueString(valueType, _nOffset, g_pListRecords->at(nRow).nSize, g_endian == XBinary::ENDIAN_BIG);
                            }
                        } else {
                            qint64 _nOffset = g_pListRecords->at(nRow).nRelOffset;
                            result = binary.read_valueString(valueType, _nOffset, g_pListRecords->at(nRow).nSize, g_endian == XBinary::ENDIAN_BIG);
                        }
                    } else if (g_valueType == XBinary::VT_SIGNATURE) {
                        if (g_pListSignatureRecords && (g_pListSignatureRecords->count() > g_pListRecords->at(nRow).nInfo)) {
                            result = g_pListSignatureRecords->at(g_pListRecords->at(nRow).nInfo).sName;
                        } else {
                            result = g_sValue;
                        }
                    } else {
                        result = g_sValue;
                    }
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                if ((nColumn == COLUMN_NUMBER) || (nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_ADDRESS) || (nColumn == COLUMN_SIZE)) {
                    result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignRight;
                } else {
                    result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignLeft;
                }
                // } else if (nRole == Qt::CheckStateRole) {
                //     if (nColumn == COLUMN_INFO) {
                //         if (g_valueType == XBinary::VT_SIGNATURE) {
                //             if (g_pListSignatureRecords && (g_pListSignatureRecords->count() > g_pListRecords->at(nRow).nInfo)) {
                //                 if (g_pListSignatureRecords->at(g_pListRecords->at(nRow).nInfo).sPatch != "") {
                //                     result = Qt::Checked;
                //                 }
                //             }
                //         }
                //     }
            } else if (nRole == Qt::UserRole + USERROLE_ADDRESS) {
                qint16 nRegionIndex = g_pListRecords->at(nRow).nRegionIndex;
                if (nRegionIndex != -1) {
                    if (g_memoryMap.listRecords.at(nRegionIndex).nAddress != -1) {
                        result = g_memoryMap.listRecords.at(nRegionIndex).nAddress + g_pListRecords->at(nRow).nRelOffset;
                    }
                }
            } else if (nRole == Qt::UserRole + USERROLE_OFFSET) {
                qint16 nRegionIndex = g_pListRecords->at(nRow).nRegionIndex;
                if (nRegionIndex != -1) {
                    if (g_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                        result = g_memoryMap.listRecords.at(nRegionIndex).nOffset + g_pListRecords->at(nRow).nRelOffset;
                    }
                } else {
                    result = g_pListRecords->at(nRow).nRelOffset;
                }
            } else if (nRole == Qt::UserRole + USERROLE_SIZE) {
                result = g_pListRecords->at(nRow).nSize;
            } else if (nRole == Qt::UserRole + USERROLE_TYPE) {
                result = g_pListRecords->at(nRow).nValueType;
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
            } else if (nSection == COLUMN_INFO) {
                // if (g_valueType == XBinary::VT_SIGNATURE) {
                //     result = tr("Patch");
                // } else {
                //     result = tr("Info");
                // }
                result = tr("Info");
            } else if (nSection == COLUMN_VALUE) {
                if (g_valueType == XBinary::VT_STRING) {
                    result = tr("String");
                } else if (g_valueType == XBinary::VT_SIGNATURE) {
                    result = tr("Signature");
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
