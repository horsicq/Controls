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
#include "xdevicetableview.h"

XDeviceTableView::XDeviceTableView(QWidget *pParent) : XAbstractTableView(pParent)
{
    g_pXInfoDB = nullptr;
    g_pDevice = nullptr;
    g_pBackupDevice = nullptr;
    g_nViewSize = 0;
    g_searchData = {};
    g_addressMode = MODE_ADDRESS;
    g_bIsReadonly = true;
    g_nVisitedIndex = 0;
}

void XDeviceTableView::setXInfoDB(XInfoDB *pXInfoDB)
{
    g_pXInfoDB = pXInfoDB;

#ifdef QT_SQL_LIB
    if (pXInfoDB) {
        connect(g_pXInfoDB, SIGNAL(reloadViewSignal()), this, SLOT(reloadView()));
    }
#endif
}

XInfoDB *XDeviceTableView::getXInfoDB()
{
    return g_pXInfoDB;
}

void XDeviceTableView::setDevice(QIODevice *pDevice)
{
    g_pDevice = pDevice;

    g_listVisited.clear();

    if (pDevice) {
        g_nViewSize = pDevice->size();
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

void XDeviceTableView::setViewSize(qint64 nViewSize)
{
    g_nViewSize = nViewSize;
}

qint64 XDeviceTableView::getViewSize()
{
    return g_nViewSize;
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

void XDeviceTableView::adjustScrollCount()
{
}

qint64 XDeviceTableView::getViewSizeByViewOffset(qint64 nViewOffset)
{
    Q_UNUSED(nViewOffset)

    return 1;
}

qint64 XDeviceTableView::addressToViewOffset(XADDR nAddress)
{
    qint64 nResult = 0;

    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);

    nResult = deviceOffsetToViewOffset(nOffset);

    return nResult;
}

XDeviceTableView::DEVICESTATE XDeviceTableView::getDeviceState(bool bGlobalOffset)
{
    DEVICESTATE result = {};
    STATE state = getState();

    result.nSelectionLocation = state.nSelectionViewOffset;
    //    result.nCursorOffset = state.nCursorViewOffset;
    result.nSelectionSize = state.nSelectionViewSize;
    result.nShowOffset = getViewOffsetStart();

    if (bGlobalOffset) {
        XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

        if (pSubDevice) {
            quint64 nInitOffset = pSubDevice->getInitLocation();
            result.nSelectionLocation += nInitOffset;
            //            result.nCursorOffset += nInitOffset;
            result.nShowOffset += nInitOffset;
        }
    }

    return result;
}

void XDeviceTableView::setDeviceState(DEVICESTATE deviceState, bool bGlobalOffset)
{
    if (bGlobalOffset) {
        XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

        if (pSubDevice) {
            quint64 nInitOffset = pSubDevice->getInitLocation();
            deviceState.nSelectionLocation -= nInitOffset;
            //            deviceState.nCursorOffset -= nInitOffset;
            deviceState.nShowOffset -= nInitOffset;
        }
    }

    _goToViewOffset(deviceState.nShowOffset);
    _initSetSelection(deviceState.nSelectionLocation, deviceState.nSelectionSize);
    //    setCursorViewOffset(deviceState.nCursorOffset);

    adjust();
    viewport()->update();
}

qint64 XDeviceTableView::deviceOffsetToViewOffset(qint64 nOffset, bool bGlobalOffset)
{
    qint64 nResult = nOffset;

    if (bGlobalOffset) {
        XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

        if (pSubDevice) {
            quint64 nInitOffset = pSubDevice->getInitLocation();
            nResult -= nInitOffset;
        }
    }

    return nResult;
}

qint64 XDeviceTableView::deviceSizeToViewSize(qint64 nOffset, qint64 nSize, bool bGlobalOffset)
{
    Q_UNUSED(nOffset)
    Q_UNUSED(bGlobalOffset)

    qint64 nResult = nSize;

    return nResult;
}

void XDeviceTableView::setDeviceSelection(qint64 nOffset, qint64 nSize)
{
    if (isSelectionEnable()) {
        qint64 nViewOffset = deviceOffsetToViewOffset(nOffset);

        _initSetSelection(nViewOffset, nSize);

        adjust();
        viewport()->update();
    }
}

qint64 XDeviceTableView::deviceOffsetToGlobal(qint64 nDeviceOffset)
{
    qint64 nResult = nDeviceOffset;

    XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

    if (pSubDevice) {
        quint64 nInitOffset = pSubDevice->getInitLocation();
        nResult += nInitOffset;
    }

    return nResult;
}

bool XDeviceTableView::isPrevVisitedAvailable()
{
    return g_nVisitedIndex > 0;
}

bool XDeviceTableView::isNextVisitedAvailable()
{
    return g_nVisitedIndex < (g_listVisited.count() - 1);
}

void XDeviceTableView::goToNextVisited()
{
    if (isNextVisitedAvailable()) {
        g_nVisitedIndex++;
        qint64 nViewOffset = g_listVisited.at(g_nVisitedIndex);

        if (_goToViewOffset(nViewOffset)) {
            _initSetSelection(nViewOffset, getViewSizeByViewOffset(nViewOffset));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::goToPrevVisited()
{
    if (isPrevVisitedAvailable()) {
        g_nVisitedIndex--;
        qint64 nViewOffset = g_listVisited.at(g_nVisitedIndex);

        if (_goToViewOffset(nViewOffset)) {
            _initSetSelection(nViewOffset, getViewSizeByViewOffset(nViewOffset));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::addVisited(qint64 nViewOffset)
{
    // #ifdef QT_DEBUG
    //     qDebug("Add visited %s", XBinary::valueToHex(nViewOffset).toLatin1().data());
    // #endif

    if ((g_listVisited.empty()) || (g_listVisited.last() != nViewOffset)) {
        qint32 nNumberOfVisited = g_listVisited.count();

        for (qint32 i = nNumberOfVisited - 1; i > g_nVisitedIndex; i--) {
            g_listVisited.removeAt(i);
        }

        g_listVisited.append(nViewOffset);

        if (g_listVisited.count() > 100) {  // TODO const
            g_listVisited.removeFirst();
        }

        g_nVisitedIndex = g_listVisited.count() - 1;

        emit visitedStateChanged();
    }
}

void XDeviceTableView::clearVisited()
{
    g_listVisited.clear();
}

QList<XDeviceTableView::HIGHLIGHTREGION> XDeviceTableView::_convertBookmarksToHighlightRegion(QList<XInfoDB::BOOKMARKRECORD> *pList)
{
    QList<HIGHLIGHTREGION> listResult;

    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        HIGHLIGHTREGION record = {};
        record.bIsValid = true;
        record.nLocation = pList->at(i).nLocation;
        record.nSize = pList->at(i).nSize;
        record.colBackground = pList->at(i).colBackground;
        record.colBackgroundSelected = getColorSelected(record.colBackground);
        record.sName = pList->at(i).sName;
        record.sComment = pList->at(i).sComment;

        listResult.append(record);
    }

    return listResult;
}

QList<XDeviceTableView::HIGHLIGHTREGION> XDeviceTableView::getHighlightRegion(QList<HIGHLIGHTREGION> *pList, quint64 nLocation)
{
    QList<HIGHLIGHTREGION> listResult;

    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((nLocation >= pList->at(i).nLocation) && (nLocation < (pList->at(i).nLocation + pList->at(i).nSize))) {
            listResult.append(pList->at(i));
        }
    }

    return listResult;
}

qint64 XDeviceTableView::write_array(qint64 nOffset, char *pData, qint64 nDataSize)
{
    //    // TODO define if XPROCESS -> use Addresses
    //    qint64 nResult = 0;

    //    if (getDevice()) {
    //        char *_pBuffer = nullptr;
    //        bool bReplaced = false;

    ////        if (XBinary::_updateReplaces(nOffset, pData, nDataSize,
    ////                                     &g_listReplaces))  // TODO optimize
    ////        {
    ////            bReplaced = true;
    ////#ifdef QT_DEBUG
    ////            qDebug("Replaced write present");
    ////#endif
    ////            _pBuffer = new char[nDataSize];

    ////            XBinary::_copyMemory(_pBuffer, pData, nDataSize);

    ////            if (XBinary::_replaceMemory(nOffset, _pBuffer, nDataSize,
    ////                                        &g_listReplaces))  // TODO optimize
    ////            {
    ////#ifdef QT_DEBUG
    ////                qDebug("Replace write");
    ////#endif
    ////            }
    ////        }

    //        _pBuffer = pData;

    //        if (saveBackup()) {
    //            nResult = XBinary::write_array(getDevice(), nOffset, _pBuffer, nDataSize);
    //        }
    //        // mb TODO error message if fails !!!

    //        if (bReplaced) {
    //            delete[] _pBuffer;
    //        }
    //    }

    //    return nResult;
    qint64 nResult = 0;

    if (getDevice()) {
        if (saveBackup()) {
            nResult = XBinary::write_array(getDevice(), nOffset, pData, nDataSize);
        }
    }

    return nResult;
}

QByteArray XDeviceTableView::read_array(qint64 nOffset, qint32 nSize)
{
    //    // TODO if device ->
    //    // TODO if XInfoDB ->
    //    QByteArray baResult;

    //    if (getDevice()) {
    //        baResult = XBinary::read_array(getDevice(), nOffset, nSize);

    ////        if (XBinary::_replaceMemory(nOffset, baResult.data(), nSize,
    ////                                    &g_listReplaces))  // TODO optimize
    ////        {
    ////#ifdef QT_DEBUG
    ////            qDebug("Replaced read present");
    ////#endif
    ////        }
    //    }

    //    return baResult;
    QByteArray baResult;

    if (getDevice()) {
        baResult = XBinary::read_array(getDevice(), nOffset, nSize);
    }

    return baResult;
}

void XDeviceTableView::goToAddress(XADDR nAddress, bool bShort, bool bAprox, bool bSaveVisited)
{
    if (nAddress != (XADDR)-1) {
        qint64 nViewOffset = addressToViewOffset(nAddress);  // deviceAddressToViewOffset

        if (bSaveVisited) {
            addVisited(getState().nSelectionViewOffset);
        }

        if (_goToViewOffset(nViewOffset, false, bShort, bAprox)) {
            if (bSaveVisited) {
                addVisited(nViewOffset);
            }

            _initSetSelection(nViewOffset, getViewSizeByViewOffset(nViewOffset));
            // TODO
        }
        // mb TODO reload
    }
}

void XDeviceTableView::goToOffset(qint64 nOffset, bool bSaveVisited)
{
    qint64 nViewOffset = deviceOffsetToViewOffset(nOffset);

    if (bSaveVisited) {
        addVisited(getState().nSelectionViewOffset);
    }

    if (_goToViewOffset(nViewOffset)) {
        if (bSaveVisited) {
            addVisited(nViewOffset);
        }

        _initSetSelection(nViewOffset, getViewSizeByViewOffset(nViewOffset));
        // TODO
    }
    // mb TODO reload
}

void XDeviceTableView::setSelectionAddress(XADDR nAddress, qint64 nSize)
{
    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);

    if (nOffset != -1) {
        qint64 nViewOffset = deviceOffsetToViewOffset(nOffset);

        _initSetSelection(nViewOffset, nSize);
        viewport()->update();  // TODO Check
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

void XDeviceTableView::setEdited(qint64 nDeviceOffset, qint64 nDeviceSize)
{
    //    QFile *pFile=dynamic_cast<QFile *>(getDevice());

    //    if(pFile)
    //    {
    //        pFile->flush();
    //    }

    updateData();

    if (g_pXInfoDB) {
        g_pXInfoDB->setEdited(nDeviceOffset, nDeviceSize);
    }

    viewport()->update();
}

void XDeviceTableView::setReadonly(bool bState)
{
    g_bIsReadonly = bState;
}

bool XDeviceTableView::isReadonly()
{
    return g_bIsReadonly;
}

void XDeviceTableView::adjustAfterAnalysis()
{
    adjustScrollCount();
    clearVisited();

    reload(true);
}

bool XDeviceTableView::isViewOffsetValid(qint64 nViewOffset)
{
    bool bResult = false;

    if ((nViewOffset >= 0) && (nViewOffset < g_nViewSize)) {
        bResult = true;
    }

    return bResult;
}

bool XDeviceTableView::isEnd(qint64 nOffset)
{
    return (nOffset == g_nViewSize);
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
        goToAddress(da.getValue(), false, false, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToOffsetSlot()
{
    DialogGoToAddress da(this, getMemoryMap(), DialogGoToAddress::TYPE_OFFSET);

    if (da.exec() == QDialog::Accepted)  // TODO use status
    {
        goToOffset(da.getValue(), true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToSelectionStart()
{
    DEVICESTATE state = getDeviceState();

    if (state.nSelectionSize) {
        goToOffset(state.nSelectionLocation, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToSelectionEnd()
{
    DEVICESTATE state = getDeviceState();

    if (state.nSelectionSize) {
        qint64 nOffset = state.nSelectionLocation + state.nSelectionSize;

        if (isEnd(nOffset)) {
            nOffset--;
        }

        // mb TODO go to end alignment
        goToOffset(nOffset, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_dumpToFileSlot()
{
    QString sSaveFileName = XBinary::getResultFileName(getDevice(), QString("%1.bin").arg(tr("Dump")));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.bin)").arg(tr("Raw data")));

    if (!sFileName.isEmpty()) {
        DEVICESTATE state = getDeviceState();

        DialogDumpProcess dd(this, getDevice(), state.nSelectionLocation, state.nSelectionSize, sFileName, DumpProcess::DT_OFFSET);

        dd.showDialogDelay();
    }
}

void XDeviceTableView::_hexSignatureSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogHexSignature dhs(this, getDevice(), state.nSelectionLocation, state.nSelectionSize);

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
    DEVICESTATE state = getDeviceState();

    g_searchData = {};
    g_searchData.nResultOffset = -1;
    g_searchData.nCurrentOffset = state.nSelectionLocation;

    DialogSearch::OPTIONS options = {};
    options.bShowBegin = true;

    DialogSearch dialogSearch(this, getDevice(), &g_searchData, mode, options);

    if (dialogSearch.exec() == QDialog::Accepted)  // TODO use status
    {
        DialogSearchProcess dsp(this, getDevice(), &g_searchData);

        dsp.showDialogDelay();

        if (g_searchData.nResultOffset != -1) {
            qint64 nViewOffset = deviceOffsetToViewOffset(g_searchData.nResultOffset);
            qint64 nViewSize = deviceSizeToViewSize(g_searchData.nResultOffset, g_searchData.nResultSize);

            _goToViewOffset(nViewOffset);
            _initSetSelection(nViewOffset, nViewSize);
            setFocus();
            viewport()->update();

        } else {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_findNextSlot()
{
    if (g_searchData.bInit) {
        g_searchData.nCurrentOffset = g_searchData.nResultOffset + 1;
        g_searchData.startFrom = XBinary::SF_CURRENTOFFSET;

        DialogSearchProcess dialogSearch(this, getDevice(), &g_searchData);

        dialogSearch.showDialogDelay();

        if (dialogSearch.isSuccess())  // TODO use status
        {
            qint64 nViewOffset = deviceOffsetToViewOffset(g_searchData.nResultOffset);
            qint64 nViewSize = deviceSizeToViewSize(g_searchData.nResultOffset, g_searchData.nResultSize);

            _goToViewOffset(nViewOffset);
            _initSetSelection(nViewOffset, nViewSize);
            setFocus();
            viewport()->update();
        } else if (g_searchData.valueType != XBinary::VT_UNKNOWN) {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_selectAllSlot()
{
    _initSetSelection(0, deviceSizeToViewSize(0, getDevice()->size()));
    viewport()->update();
}

void XDeviceTableView::_copyDataSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogShowData dialogShowData(this, getDevice(), state.nSelectionLocation, state.nSelectionSize);

    dialogShowData.exec();
}

void XDeviceTableView::_copyAddressSlot()
{
    DEVICESTATE state = getDeviceState();

    XADDR nAddress = XBinary::offsetToAddress(getMemoryMap(), state.nSelectionLocation);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyRelAddressSlot()
{
    DEVICESTATE state = getDeviceState();

    XADDR nAddress = XBinary::offsetToRelAddress(getMemoryMap(), state.nSelectionLocation);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyOffsetSlot()
{
    DEVICESTATE state = getDeviceState();

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, state.nSelectionLocation));
}

void XDeviceTableView::_setEdited(qint64 nDeviceOffset, qint64 nDeviceSize)
{
    setEdited(nDeviceOffset, nDeviceSize);

    emit dataChanged(nDeviceOffset, nDeviceSize);
}

void XDeviceTableView::goToAddressSlot(XADDR nAddress, qint64 nSize)
{
    Q_UNUSED(nSize)

    goToAddress(nAddress, true, true, true);
    reload(true);
}

void XDeviceTableView::reloadView()
{
    updateData();

    viewport()->update();
}
