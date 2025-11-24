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
#ifndef XMODEL_ARCHIVERECORDS_H
#define XMODEL_ARCHIVERECORDS_H

#include "xbinary.h"
#include "xmodel.h"

class XModel_ArchiveRecords : public XModel {
    Q_OBJECT

public:
    // Make dynamic
    enum COLUMN {
        COLUMN_NAME = 0,
        COLUMN_STREAMOFFSET,
        COLUMN_STREAMSIZE,
        // COLUMN_DECOMPRESSEDOFFSET,
        // COLUMN_DECOMPRESSEDSIZE,
        COLUMN_COMPRESSMETHOD,
        COLUMN_CRC,
        COLUMN_DATETIME,
        __COLUMN_COUNT
    };

    explicit XModel_ArchiveRecords(QList<XBinary::ARCHIVERECORD> *pListArchiveRecords, QObject *pParent = nullptr);

    QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const override;

private:
    QList<XBinary::ARCHIVERECORD> *m_pListArchiveRecords;
    void _initColumns();
};

#endif  // XMODEL_ARCHIVERECORDS_H
