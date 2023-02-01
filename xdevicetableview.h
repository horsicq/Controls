/* Copyright (c) 2020-2023 hors<horsicq@gmail.com>
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
#ifndef XDEVICETABLEVIEW_H
#define XDEVICETABLEVIEW_H

#include "dialogdumpprocess.h"
#include "dialoggotoaddress.h"
#include "dialoghexsignature.h"
#include "dialogsearch.h"
#include "dialogshowdata.h"
#include "dialogsearchprocess.h"
#include "xabstracttableview.h"
#include "xformats.h"
#include "xinfodb.h"

class XDeviceTableView : public XAbstractTableView {
    Q_OBJECT

public:
    enum MODE {
        MODE_ADDRESS = 0,
        MODE_OFFSET,
        MODE_RELADDRESS,
        MODE_THIS
    };

    XDeviceTableView(QWidget *pParent = nullptr);

    void setXInfoDB(XInfoDB *pXInfoDB);
    XInfoDB *getXInfoDB();
    void setDevice(QIODevice *pDevice);
    void setBackupDevice(QIODevice *pDevice);
    QIODevice *getDevice();
    QIODevice *getBackupDevice();
    void setViewSize(qint64 nViewSize);
    qint64 getViewSize();
    void setMemoryMap(XBinary::_MEMORY_MAP memoryMap);
    XBinary::_MEMORY_MAP *getMemoryMap();
    void setAddressMode(MODE addressMode);
    MODE getAddressMode();
    qint64 write_array(qint64 nOffset, char *pData, qint64 nDataSize);
    QByteArray read_array(qint64 nOffset, qint32 nSize);
    bool isReplaced(qint64 nOffset, qint32 nSize);
    void goToAddress(XADDR nAddress, bool bShort = false, bool bAprox = false);
    void goToOffset(qint64 nOffset);
    void setSelectionAddress(XADDR nAddress, qint64 nSize);
    bool isEdited();
    bool saveBackup();
    void setReadonly(bool bState);
    bool isReadonly();
    bool isAnalyzed();
    void adjustAfterAnalysis();

public slots:
    void setEdited();

protected:
    virtual bool isViewOffsetValid(qint64 nOffset);
    virtual bool isEnd(qint64 nOffset);
    void setMemoryReplaces(QList<XBinary::MEMORY_REPLACE> listReplaces); // TODO Check
    virtual void adjustLineCount();

protected slots:
    void _goToAddressSlot();
    void _goToOffsetSlot();
    void _goToSelectionStart();
    void _goToSelectionEnd();
    void _dumpToFileSlot();
    void _hexSignatureSlot();
    void _findStringSlot();
    void _findSignatureSlot();
    void _findValueSlot();
    void _findSlot(DialogSearch::SEARCHMODE mode);
    void _findNextSlot();
    void _selectAllSlot();
    void _copyDataSlot();
    void _copyAddressSlot();
    void _copyRelAddressSlot();
    void _copyOffsetSlot();
    void _setEdited();

private:
    XInfoDB *g_pXInfoDB;
    QIODevice *g_pDevice;
    QIODevice *g_pBackupDevice;
    qint64 g_nViewSize;
    XBinary::_MEMORY_MAP g_memoryMap;
    XBinary::SEARCHDATA g_searchData;
    QList<XBinary::MEMORY_REPLACE> g_listReplaces;  // TODO move to g_pXInfoDB !!!
    MODE g_addressMode;
    bool g_bIsReadonly;
};

#endif  // XDEVICETABLEVIEW_H
