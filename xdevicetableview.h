// copyright (c) 2020-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef XDEVICETABLEVIEW_H
#define XDEVICETABLEVIEW_H

#include "xabstracttableview.h"
#include "xformats.h"
#include "dialoggotoaddress.h"
#include "dialogsearch.h"
#include "dialogdumpprocess.h"
#include "dialogsearchprocess.h"
#include "dialoghexsignature.h"

class XDeviceTableView : public XAbstractTableView
{
    Q_OBJECT

public:
    enum MODE
    {
        MODE_ADDRESS,
        MODE_RELADDRESS
    };

    XDeviceTableView(QWidget *pParent=nullptr);
    void setDevice(QIODevice *pDevice);
    QIODevice *getDevice();
    qint64 getDataSize();
    void setMemoryMap(XBinary::_MEMORY_MAP memoryMap);
    XBinary::_MEMORY_MAP *getMemoryMap();
    void setSignaturesPath(QString sSignaturesPath);
    void setAddressMode(MODE addressMode);
    MODE getAddressMode();

    void setMemoryReplaces(QList<XBinary::MEMORY_REPLACE> listReplaces);

    QByteArray read_array(qint64 nOffset,qint32 nSize);
    bool isReplaced(qint64 nOffset,qint32 nSize);

    void goToAddress(qint64 nAddress);
    void goToOffset(qint64 nOffset);
    void setSelectionAddress(qint64 nAddress,qint64 nSize);

protected:
    virtual bool isOffsetValid(qint64 nOffset);
    virtual bool isEnd(qint64 nOffset);

protected slots:
    void _goToAddressSlot();
    void _goToOffsetSlot();
    void _dumpToFileSlot();
    void _hexSignatureSlot();
    void _findSlot();
    void _findNextSlot();
    void _selectAllSlot();
    void _copyAsHexSlot();
    void _copyCursorAddressSlot();
    void _copyCursorOffsetSlot();

private:
    QIODevice *g_pDevice;
    qint64 g_nDataSize;
    XBinary::_MEMORY_MAP g_memoryMap;
    SearchProcess::SEARCHDATA g_searchData;
    QList<XBinary::MEMORY_REPLACE> g_listReplaces;
    QString g_sSignaturesPath;
    MODE g_addressMode;
};

#endif // XDEVICETABLEVIEW_H
