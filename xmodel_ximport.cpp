/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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
#include "xmodel_ximport.h"

XModel_XImport::XModel_XImport(QVector<XBinary::XIMPORT_STRUCT> *pListImports, QObject *pParent) : XModel(pParent)
{
    m_pListImports = pListImports;
    if (pListImports) {
        _setRowCount(pListImports->count());
    } else {
        _setRowCount(0);
    }
    _setColumnCount(__COLUMN_COUNT);
    _initColumns();
}

void XModel_XImport::_initColumns()
{
    setColumnName(COLUMN_LIBRARY, QObject::tr("Library"));
    setColumnName(COLUMN_FUNCTION, QObject::tr("Function"));
    setColumnName(COLUMN_ORDINAL, QObject::tr("Ordinal"));
    setColumnName(COLUMN_OFFSET, QObject::tr("Offset"));
    setColumnName(COLUMN_ADDRESS, QObject::tr("Address"));

    setColumnAlignment(COLUMN_LIBRARY, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_FUNCTION, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_ORDINAL, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_OFFSET, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_ADDRESS, Qt::AlignVCenter | Qt::AlignRight);

    setColumnSymbolSize(COLUMN_LIBRARY, 20);
    setColumnSymbolSize(COLUMN_FUNCTION, 30);
    setColumnSymbolSize(COLUMN_ORDINAL, 8);
    setColumnSymbolSize(COLUMN_OFFSET, 16);
    setColumnSymbolSize(COLUMN_ADDRESS, 16);
}

QVariant XModel_XImport::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!m_pListImports) {
        return result;
    }

    if (index.isValid()) {
        qint32 nRow = index.row();
        if (nRow >= 0 && nRow < m_pListImports->count()) {
            qint32 nColumn = index.column();
            const XBinary::XIMPORT_STRUCT &rec = m_pListImports->at(nRow);
            if (nRole == Qt::DisplayRole) {
                if (nColumn == COLUMN_LIBRARY) {
                    result = rec.sLibrary;
                } else if (nColumn == COLUMN_FUNCTION) {
                    result = rec.sFunction;
                } else if (nColumn == COLUMN_ORDINAL) {
                    if (rec.nOrdinal >= 0) {
                        result = rec.nOrdinal;
                    }
                } else if (nColumn == COLUMN_OFFSET) {
                    result = QString::number(rec.nOffset, 16);
                } else if (nColumn == COLUMN_ADDRESS) {
                    result = QString::number(rec.nAddress, 16);
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            } else if (nRole >= Qt::UserRole) {
                if (nRole == (Qt::UserRole + XModel::USERROLE_ORIGINDEX)) {
                    result = nRow;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_OFFSET)) {
                    result = rec.nOffset;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_SIZE)) {
                    result = rec.nSize;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_ADDRESS)) {
                    result = (qulonglong)rec.nAddress;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING1)) {
                    result = rec.sLibrary;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING2)) {
                    result = rec.sFunction;
                }
            }
        }
    }

    return result;
}

QVariant XModel_XImport::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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

XModel::SORT_METHOD XModel_XImport::getSortMethod(qint32 nColumn)
{
    SORT_METHOD result = SORT_METHOD_DEFAULT;

    if ((nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_ADDRESS)) {
        result = SORT_METHOD_HEX;
    }

    return result;
}

bool XModel_XImport::isCustomFilter()
{
    return false;
}

bool XModel_XImport::isCustomSort()
{
    return false;
}
