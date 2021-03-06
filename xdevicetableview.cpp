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
#include "xdevicetableview.h"

XDeviceTableView::XDeviceTableView(QWidget *pParent) : XAbstractTableView(pParent)
{
    g_pDevice=nullptr;
    g_nDataSize=0;
    g_searchData={};
    g_addressMode=MODE_ADDRESS;
}

void XDeviceTableView::setDevice(QIODevice *pDevice)
{
    g_pDevice=pDevice;
    g_nDataSize=pDevice->size();
}

QIODevice *XDeviceTableView::getDevice()
{
    return g_pDevice;
}

qint64 XDeviceTableView::getDataSize()
{
    return g_nDataSize;
}

void XDeviceTableView::setMemoryMap(XBinary::_MEMORY_MAP memoryMap)
{
    if(memoryMap.fileType==XBinary::FT_UNKNOWN)
    {
        XBinary binary(getDevice());
        memoryMap=binary.getMemoryMap();
    }

    g_memoryMap=memoryMap;
}

XBinary::_MEMORY_MAP *XDeviceTableView::getMemoryMap()
{
    return &g_memoryMap;
}

void XDeviceTableView::setSignaturesPath(QString sSignaturesPath)
{
    g_sSignaturesPath=sSignaturesPath;
}

void XDeviceTableView::setAddressMode(XDeviceTableView::MODE addressMode)
{
    g_addressMode=addressMode;
}

XDeviceTableView::MODE XDeviceTableView::getAddressMode()
{
    return g_addressMode;
}

void XDeviceTableView::setMemoryReplaces(QList<XBinary::MEMORY_REPLACE> listReplaces)
{
    int nNumberOfRecords=listReplaces.count();

    for(int i=0;i<nNumberOfRecords;i++)
    {
        listReplaces[i].nOffset=XBinary::addressToOffset(&g_memoryMap,listReplaces.at(i).nAddress);
    }

    g_listReplaces=listReplaces;
}

QByteArray XDeviceTableView::read_array(qint64 nOffset, qint32 nSize)
{
    QByteArray baResult=XBinary::read_array(getDevice(),nOffset,nSize);

    if(XBinary::_replaceMemory(nOffset,baResult.data(),nSize,&g_listReplaces))
    {
        qDebug("Replaced"); // TODO remove
    }

    return baResult;
}

bool XDeviceTableView::isReplaced(qint64 nOffset, qint32 nSize)
{
    return XBinary::_isReplaced(nOffset,nSize,&g_listReplaces);
}

void XDeviceTableView::goToAddress(qint64 nAddress)
{
    qint64 nOffset=XBinary::addressToOffset(getMemoryMap(),nAddress);
    _goToOffset(nOffset); // TODO Check
    // TODO reload
}

void XDeviceTableView::goToOffset(qint64 nOffset)
{
    _goToOffset(nOffset);
    // TODO reload
}

void XDeviceTableView::setSelectionAddress(qint64 nAddress, qint64 nSize)
{
    qint64 nOffset=XBinary::addressToOffset(getMemoryMap(),nAddress);

    if(nOffset!=-1)
    {
        setSelection(nOffset,nSize);
    }
}

bool XDeviceTableView::isOffsetValid(qint64 nOffset)
{
    bool bResult=false;

    if((nOffset>=0)&&(nOffset<g_nDataSize))
    {
        bResult=true;
    }

    return bResult;
}

bool XDeviceTableView::isEnd(qint64 nOffset)
{
    return (nOffset==g_nDataSize);
}

void XDeviceTableView::_goToAddressSlot()
{
    DialogGoToAddress::TYPE type=DialogGoToAddress::TYPE_ADDRESS;

    if(g_addressMode==MODE_RELADDRESS)
    {
        type=DialogGoToAddress::TYPE_RELADDRESS;
    }

    DialogGoToAddress da(this,getMemoryMap(),type);
    if(da.exec()==QDialog::Accepted)
    {
        goToAddress(da.getValue());
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToOffsetSlot()
{
    DialogGoToAddress da(this,getMemoryMap(),DialogGoToAddress::TYPE_OFFSET);
    if(da.exec()==QDialog::Accepted)
    {
        goToOffset(da.getValue());
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_dumpToFileSlot()
{
    QString sFilter;
    sFilter+=QString("%1 (*.bin)").arg(tr("Raw data"));
    QString sSaveFileName=XBinary::getResultFileName(getDevice(),"dump.bin");
    QString sFileName=QFileDialog::getSaveFileName(this,tr("Save dump"),sSaveFileName,sFilter);

    if(!sFileName.isEmpty())
    {
        STATE state=getState();

        DialogDumpProcess dd(this,getDevice(),state.nSelectionOffset,state.nSelectionSize,sFileName,DumpProcess::DT_OFFSET);

        dd.exec();
    }
}

void XDeviceTableView::_hexSignatureSlot()
{
    STATE state=getState();

    DialogHexSignature dhs(this,getDevice(),state.nSelectionOffset,state.nSelectionSize,g_sSignaturesPath);

    dhs.setShortcuts(getShortcuts());

    dhs.exec();
}

void XDeviceTableView::_findSlot()
{
    STATE state=getState();

    g_searchData={};
    g_searchData.nResultOffset=-1;
    g_searchData.nCurrentOffset=state.nCursorOffset;

    DialogSearch dialogSearch(this,getDevice(),&g_searchData);

    if(dialogSearch.exec()==QDialog::Accepted)
    {
        _goToOffset(g_searchData.nResultOffset);
        setSelection(g_searchData.nResultOffset,g_searchData.nResultSize);
        setFocus();
        viewport()->update();
    }
    else if(g_searchData.type!=SearchProcess::TYPE_UNKNOWN)
    {
        emit errorMessage(tr("Nothing found"));
    }
}

void XDeviceTableView::_findNextSlot()
{
    if(g_searchData.bInit)
    {
        g_searchData.nCurrentOffset=g_searchData.nResultOffset+1;
        g_searchData.startFrom=SearchProcess::SF_CURRENTOFFSET;

        DialogSearchProcess dialogSearch(this,getDevice(),&g_searchData);

        if(dialogSearch.exec()==QDialog::Accepted)
        {
            _goToOffset(g_searchData.nResultOffset);
            setSelection(g_searchData.nResultOffset,g_searchData.nResultSize);
            setFocus();
            viewport()->update();
        }
        else if(g_searchData.type!=SearchProcess::TYPE_UNKNOWN)
        {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_selectAllSlot()
{
    setSelection(0,getDevice()->size());
}

void XDeviceTableView::_copyAsHexSlot()
{
    STATE state=getState();

    qint64 nSize=qMin(state.nSelectionSize,(qint64)0x10000);

    QByteArray baData=read_array(state.nSelectionOffset,nSize);

    QApplication::clipboard()->setText(baData.toHex());
}

void XDeviceTableView::_copyCursorAddressSlot()
{
    STATE state=getState();

    qint64 nAddress=0;

    if(g_addressMode==MODE_ADDRESS)
    {
        nAddress=XBinary::offsetToAddress(getMemoryMap(),state.nCursorOffset);
    }
    else if(g_addressMode==MODE_RELADDRESS)
    {
        nAddress=XBinary::offsetToRelAddress(getMemoryMap(),state.nCursorOffset);
    }

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN,nAddress));
}

void XDeviceTableView::_copyCursorOffsetSlot()
{
    STATE state=getState();

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN,state.nCursorOffset));
}
