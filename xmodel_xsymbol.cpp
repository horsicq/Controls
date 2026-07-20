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
#include "xmodel_xsymbol.h"

XModel_XSymbol::XModel_XSymbol(QVector<XBinary::XSYMBOL_STRUCT> *pListSymbols, QObject *pParent) : XModel(pParent)
{
    m_pListSymbols = pListSymbols;
    if (pListSymbols) {
        _setRowCount(pListSymbols->count());
    } else {
        _setRowCount(0);
    }
    _setColumnCount(__COLUMN_COUNT);
    _initColumns();
}

void XModel_XSymbol::_initColumns()
{
    setColumnName(COLUMN_NAME, QObject::tr("Name"));
    setColumnName(COLUMN_TYPE, QObject::tr("Type"));
    setColumnName(COLUMN_OFFSET, QObject::tr("Offset"));
    setColumnName(COLUMN_SIZE, QObject::tr("Size"));
    setColumnName(COLUMN_ADDRESS, QObject::tr("Address"));

    setColumnAlignment(COLUMN_NAME, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_TYPE, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_OFFSET, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_SIZE, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_ADDRESS, Qt::AlignVCenter | Qt::AlignRight);

    setColumnSymbolSize(COLUMN_NAME, 30);
    setColumnSymbolSize(COLUMN_TYPE, 12);
    setColumnSymbolSize(COLUMN_OFFSET, 16);
    setColumnSymbolSize(COLUMN_SIZE, 16);
    setColumnSymbolSize(COLUMN_ADDRESS, 16);
}

QString XModel_XSymbol::symbolTypeToString(XBinary::SYMBOL_TYPE symbolType)
{
    QString sResult = QObject::tr("Unknown");

    if (symbolType == XBinary::SYMBOL_TYPE_EXPORT) {
        sResult = QObject::tr("Export");
    } else if (symbolType == XBinary::SYMBOL_TYPE_IMPORT) {
        sResult = QObject::tr("Import");
    } else if (symbolType == XBinary::SYMBOL_TYPE_LABEL) {
        sResult = QObject::tr("Label");
    } else if (symbolType == XBinary::SYMBOL_TYPE_ANSISTRING) {
        sResult = QObject::tr("Ansi string");
    } else if (symbolType == XBinary::SYMBOL_TYPE_UNICODESTRING) {
        sResult = QObject::tr("Unicode string");
    }

    return sResult;
}

QVariant XModel_XSymbol::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!m_pListSymbols) {
        return result;
    }

    if (index.isValid()) {
        qint32 nRow = index.row();
        if (nRow >= 0 && nRow < m_pListSymbols->count()) {
            qint32 nColumn = index.column();
            const XBinary::XSYMBOL_STRUCT &rec = m_pListSymbols->at(nRow);
            if (nRole == Qt::DisplayRole) {
                if (nColumn == COLUMN_NAME) {
                    result = rec.sName;
                } else if (nColumn == COLUMN_TYPE) {
                    result = symbolTypeToString(rec.symbolType);
                } else if (nColumn == COLUMN_OFFSET) {
                    result = QString::number(rec.nOffset, 16);
                } else if (nColumn == COLUMN_SIZE) {
                    result = QString::number(rec.nSize, 16);
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
                    result = rec.sName;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING2)) {
                    result = symbolTypeToString(rec.symbolType);
                }
            }
        }
    }

    return result;
}

QVariant XModel_XSymbol::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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

XModel::SORT_METHOD XModel_XSymbol::getSortMethod(qint32 nColumn)
{
    SORT_METHOD result = SORT_METHOD_DEFAULT;

    if ((nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_SIZE) || (nColumn == COLUMN_ADDRESS)) {
        result = SORT_METHOD_HEX;
    }

    return result;
}

bool XModel_XSymbol::isCustomFilter()
{
    return false;
}

bool XModel_XSymbol::isCustomSort()
{
    return false;
}
