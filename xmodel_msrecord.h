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

#ifndef XMODEL_MSRECORD_H
#define XMODEL_MSRECORD_H

#include "xbinary.h"
#include <QAbstractItemModel>

class XModel_MSRecord : public QAbstractItemModel {
public:
    enum COLUMN {
        COLUMN_NUMBER,
        COLUMN_OFFSET,
        COLUMN_ADDRESS,
        COLUMN_REGION,
        COLUMN_SIZE,
        COLUMN_TYPE,
        COLUMN_VALUE,
        __COLUMN_SIZE
    };

    enum USERROLE {
        USERROLE_SIZE = 0,
        USERROLE_OFFSET,
        USERROLE_ADDRESS,
        USERROLE_TYPE
    };

    XModel_MSRecord(QIODevice *pDevice, const XBinary::_MEMORY_MAP &memoryMap, QVector<XBinary::MS_RECORD> *pListRecods, XBinary::VT valueType,
                    QObject *pParent = nullptr);

    void setValue(XBinary::ENDIAN endian, XBinary::VT valueType, QVariant varValue);
    void setSignaturesList(QList<XBinary::SIGNATUREDB_RECORD> *pListSignatureRecords);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    virtual QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const;
    virtual void sort(int nColumn, Qt::SortOrder sortOrder = Qt::AscendingOrder);
    qint32 getColumnSymbolSize(qint32 nColumn);

private:
    QIODevice *g_pDevice;
    XBinary::_MEMORY_MAP g_memoryMap;
    QVector<XBinary::MS_RECORD> *g_pListRecords;
    qint32 g_nRowCount;
    qint32 g_nColumnCount;
    qint32 g_nColumnWidths[__COLUMN_SIZE];
    XBinary::MODE g_modeAddress;
    XBinary::MODE g_modeOffset;
    XBinary::ENDIAN g_endian;
    QVariant g_varValue;
    XBinary::VT g_valueType;
    QString g_sValue;
    QList<XBinary::SIGNATUREDB_RECORD> *g_pListSignatureRecords;
};

#endif  // XMODEL_MSRECORD_H
