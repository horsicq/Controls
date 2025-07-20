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
#ifndef XMODEL_BINARY_H
#define XMODEL_BINARY_H

#include "xbinary.h"
#include "xmodel.h"

class XModel_Binary : public XModel {
    Q_OBJECT

    enum HEADER_COLUMN {
        HEADER_COLUMN_NAME = 0,
        HEADER_COLUMN_OFFSET,
        HEADER_COLUMN_SIZE,
        HEADER_COLUMN_TYPE,
        HEADER_COLUMN_VALUE,
        HEADER_COLUMN_INFO,
        HEADER_COLUMN_COMMENT,
        __HEADER_COLUMN_SIZE
    };

public:
    XModel_Binary(const XBinary::DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<XBinary::DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles,
                  QObject *pParent);

    virtual QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    virtual QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const;
    void setComments(QList<QString> *pListComments);

private:
    XBinary::DATA_RECORDS_OPTIONS g_dataRecordsOptions;
    QList<XBinary::DATA_RECORD_ROW> *g_pListDataRecords;
    QList<QString> *g_pListTitles;
    QList<QString> *g_pListComments;
};

#endif  // XMODEL_BINARY_H
