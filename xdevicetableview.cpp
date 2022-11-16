/* Copyright (c) 2020-2022 hors<horsicq@gmail.com>
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
#include "xdevicetableview.h"

XDeviceTableView::XDeviceTableView(QWidget *pParent) : XAbstractTableView(pParent)
{
    g_pXInfoDB = nullptr;
    g_pDevice = nullptr;
    g_pBackupDevice = nullptr;
    g_nDataSize = 0;
    g_searchData = {};
    g_addressMode = MODE_ADDRESS;
    g_bIsReadonly = true;
}

void XDeviceTableView::setXInfoDB(XInfoDB *pXInfoDB)
{
    g_pXInfoDB = pXInfoDB;
}

XInfoDB *XDeviceTableView::getXInfoDB()
{
    return g_pXInfoDB;
}

void XDeviceTableView::setDevice(QIODevice *pDevice)
{
    g_pDevice = pDevice;

    if (pDevice) {
        g_nDataSize = pDevice->size();
        //    setReadonly(!(pDevice->isWritable()));
        setActive(true);
    } else {
        setActive(false);
    }
}

void XDeviceTableView::setBackupDevice(QIODevice *pDevice)
{
    g_pBackupDevice = pDevice;
}

QIODevice *XDeviceTableView::getDevice()
{
    return g_pDevice;
}

QIODevice *XDeviceTableView::getBackupDevice()
{
    QIODevice *pResult = nullptr;

    if (g_pBackupDevice) {
        pResult = g_pBackupDevice;
    } else {
        pResult = g_pDevice;
    }

    return pResult;
}

qint64 XDeviceTableView::getDataSize()
{
    return g_nDataSize;
}

void XDeviceTableView::setMemoryMap(XBinary::_MEMORY_MAP memoryMap)
{
    if (memoryMap.fileType == XBinary::FT_UNKNOWN) {
        XBinary binary(getDevice());
        memoryMap = binary.getMemoryMap();
    }

    g_memoryMap = memoryMap;
}

XBinary::_MEMORY_MAP *XDeviceTableView::getMemoryMap()
{
    return &g_memoryMap;
}

void XDeviceTableView::setAddressMode(XDeviceTableView::MODE addressMode)
{
    g_addressMode = addressMode;
}

XDeviceTableView::MODE XDeviceTableView::getAddressMode()
{
    return g_addressMode;
}

void XDeviceTableView::setMemoryReplaces(QList<XBinary::MEMORY_REPLACE> listReplaces)
{
    qint32 nNumberOfRecords = listReplaces.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        listReplaces[i].nOffset = XBinary::addressToOffset(&g_memoryMap, listReplaces.at(i).nAddress);
    }

    g_listReplaces = listReplaces;
}

qint64 XDeviceTableView::write_array(qint64 nOffset, char *pData, qint64 nDataSize)
{
    qint64 nResult = 0;

    if (getDevice()) {
        char *_pBuffer = nullptr;
        bool bReplaced = false;

        if (XBinary::_updateReplaces(nOffset, pData, nDataSize,
                                     &g_listReplaces))  // TODO optimize
        {
            bReplaced = true;
#ifdef QT_DEBUG
            qDebug("Replaced write present");
#endif
            _pBuffer = new char[nDataSize];

            XBinary::_copyMemory(_pBuffer, pData, nDataSize);

            if (XBinary::_replaceMemory(nOffset, _pBuffer, nDataSize,
                                        &g_listReplaces))  // TODO optimize
            {
#ifdef QT_DEBUG
                qDebug("Replace write");
#endif
            }
        }

        _pBuffer = pData;

        if (saveBackup()) {
            nResult = XBinary::write_array(getDevice(), nOffset, _pBuffer, nDataSize);
        }
        // mb TODO error message if fails !!!

        if (bReplaced) {
            delete[] _pBuffer;
        }
    }

    return nResult;
}

QByteArray XDeviceTableView::read_array(qint64 nOffset, qint32 nSize)
{
    QByteArray baResult;

    if (getDevice()) {
        baResult = XBinary::read_array(getDevice(), nOffset, nSize);

        if (XBinary::_replaceMemory(nOffset, baResult.data(), nSize,
                                    &g_listReplaces))  // TODO optimize
        {
#ifdef QT_DEBUG
            qDebug("Replaced read present");
#endif
        }
    }

    return baResult;
}

bool XDeviceTableView::isReplaced(qint64 nOffset, qint32 nSize)
{
    return XBinary::_isReplaced(nOffset, nSize, &g_listReplaces);
}

void XDeviceTableView::goToAddress(XADDR nAddress, bool bShort, bool bAprox)
{
    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);
    _goToOffset(nOffset, false, bShort, bAprox);  // TODO Check
    // mb TODO reload
}

void XDeviceTableView::goToOffset(qint64 nOffset)
{
    _goToOffset(nOffset);
    // mb TODO reload
}

void XDeviceTableView::setSelectionAddress(XADDR nAddress, qint64 nSize)
{
    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);

    if (nOffset != -1) {
        setSelection(nOffset, nSize);
    }
}

bool XDeviceTableView::isEdited()
{
    bool bResult = XBinary::isBackupPresent(getBackupDevice());

    return bResult;
}

bool XDeviceTableView::saveBackup()
{
    bool bResult = true;

    if ((getGlobalOptions()->isSaveBackup()) && (!isEdited())) {
        // Save backup
        bResult = XBinary::saveBackup(getBackupDevice());
    }

    return bResult;
}

void XDeviceTableView::setEdited()
{
    //    QFile *pFile=dynamic_cast<QFile *>(getDevice());

    //    if(pFile)
    //    {
    //        pFile->flush();
    //    }

    updateData();

    if (g_pXInfoDB) {
        g_pXInfoDB->reload(false);
    }

    //    viewport()->update();
}

void XDeviceTableView::setReadonly(bool bState)
{
    g_bIsReadonly = bState;
}

bool XDeviceTableView::isReadonly()
{
    return g_bIsReadonly;
}

bool XDeviceTableView::isOffsetValid(qint64 nOffset)
{
    bool bResult = false;

    if ((nOffset >= 0) && (nOffset < g_nDataSize)) {
        bResult = true;
    }

    return bResult;
}

bool XDeviceTableView::isEnd(qint64 nOffset)
{
    return (nOffset == g_nDataSize);
}

void XDeviceTableView::_goToAddressSlot()
{
    DialogGoToAddress::TYPE type = DialogGoToAddress::TYPE_VIRTUALADDRESS;

    if (g_addressMode == MODE_RELADDRESS) {
        type = DialogGoToAddress::TYPE_RELVIRTUALADDRESS;
    }

    DialogGoToAddress da(this, getMemoryMap(), type);

    if (da.exec() == QDialog::Accepted)  // TODO use status
    {
        goToAddress(da.getValue());
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToOffsetSlot()
{
    DialogGoToAddress da(this, getMemoryMap(), DialogGoToAddress::TYPE_OFFSET);

    if (da.exec() == QDialog::Accepted)  // TODO use status
    {
        goToOffset(da.getValue());
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_dumpToFileSlot()
{
    QString sSaveFileName = XBinary::getResultFileName(getDevice(), QString("%1.bin").arg(tr("Dump")));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.bin)").arg(tr("Raw data")));

    if (!sFileName.isEmpty()) {
        STATE state = getState();

        DialogDumpProcess dd(this, getDevice(), state.nSelectionOffset, state.nSelectionSize, sFileName, DumpProcess::DT_OFFSET);

        dd.showDialogDelay(1000);
    }
}

void XDeviceTableView::_hexSignatureSlot()
{
    STATE state = getState();

    DialogHexSignature dhs(this, getDevice(), state.nSelectionOffset, state.nSelectionSize);

    dhs.setGlobal(getShortcuts(), getGlobalOptions());

    dhs.exec();
}

void XDeviceTableView::_findStringSlot()
{
    _findSlot(DialogSearch::SEARCHMODE_STRING);
}

void XDeviceTableView::_findSignatureSlot()
{
    _findSlot(DialogSearch::SEARCHMODE_SIGNATURE);
}

void XDeviceTableView::_findValueSlot()
{
    _findSlot(DialogSearch::SEARCHMODE_VALUE);
}

void XDeviceTableView::_findSlot(DialogSearch::SEARCHMODE mode)
{
    STATE state = getState();

    g_searchData = {};
    g_searchData.nResultOffset = -1;
    g_searchData.nCurrentOffset = state.nCursorOffset;

    DialogSearch dialogSearch(this, getDevice(), &g_searchData, mode);

    if (dialogSearch.exec() == QDialog::Accepted)  // TODO use status
    {
        _goToOffset(g_searchData.nResultOffset);
        setSelection(g_searchData.nResultOffset, g_searchData.nResultSize);
        setFocus();
        viewport()->update();
    } else if (g_searchData.type != SearchProcess::TYPE_UNKNOWN) {
        emit errorMessage(tr("Nothing found"));
    }
}

void XDeviceTableView::_findNextSlot()
{
    if (g_searchData.bInit) {
        g_searchData.nCurrentOffset = g_searchData.nResultOffset + 1;
        g_searchData.startFrom = SearchProcess::SF_CURRENTOFFSET;

        DialogSearchProcess dialogSearch(this, getDevice(), &g_searchData);

        dialogSearch.showDialogDelay(1000);

        if (dialogSearch.isSuccess())  // TODO use status
        {
            _goToOffset(g_searchData.nResultOffset);
            setSelection(g_searchData.nResultOffset, g_searchData.nResultSize);
            setFocus();
            viewport()->update();
        } else if (g_searchData.type != SearchProcess::TYPE_UNKNOWN) {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_selectAllSlot()
{
    setSelection(0, getDevice()->size());
}

void XDeviceTableView::_copyHexSlot()
{
    STATE state = getState();

    qint64 nSize = qMin(state.nSelectionSize, (qint64)0x10000);

    QByteArray baData = read_array(state.nSelectionOffset, nSize);

    QApplication::clipboard()->setText(baData.toHex());
}

void XDeviceTableView::_copyAddressSlot()
{
    STATE state = getState();

    XADDR nAddress = XBinary::offsetToAddress(getMemoryMap(), state.nCursorOffset);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyRelAddressSlot()
{
    STATE state = getState();

    XADDR nAddress = XBinary::offsetToRelAddress(getMemoryMap(), state.nCursorOffset);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyOffsetSlot()
{
    STATE state = getState();

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, state.nCursorOffset));
}

void XDeviceTableView::_setEdited()
{
    setEdited();

    emit dataChanged();
}
