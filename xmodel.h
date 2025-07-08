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

#ifndef XMODEL_H
#define XMODEL_H

#include <QAbstractItemModel>

class XModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum SORT_METHOD {
        SORT_METHOD_DEFAULT,
        SORT_METHOD_HEX,
    };

    enum USERROLE {
        USERROLE_SIZE = 0,
        USERROLE_OFFSET,
        USERROLE_ADDRESS,
        USERROLE_TYPE
    };

    XModel(QObject *pParent);

    void setColumnSymbolSize(qint32 nColumn, qint32 nValue);
    qint32 getColumnSymbolSize(qint32 nColumn);
    virtual SORT_METHOD getSortMethod(qint32 nColumn);
    virtual bool isCustomFilter();
    virtual bool isCustomSort();
    void setRowHidden(qint32 nRow, bool bState);
    void setRowPrio(qint32 nRow, quint64 nPrio);
    bool isRowHidden(qint32 nRow);
    quint64 getRowPrio(qint32 nRow);
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    void _setRowCount(qint32 nRowCount);
    void _setColumnCount(qint32 nColumnCount);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    QHash<qint32, bool> g_hashRowHidden;
    QHash<qint32, quint64> g_hashRowPrio;
    QHash<qint32, qint32> g_hashColumnSymbolSize;
    qint32 g_nRowCount;
    qint32 g_nColumnCount;
};

#endif  // XMODEL_H
