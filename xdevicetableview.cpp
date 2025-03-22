/* Copyright (c) 2020-2025 hors<horsicq@gmail.com>
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
    g_nViewSize = 0;
    g_searchData = {};
    g_locationMode = LOCMODE_ADDRESS;
    g_nLocationBase = 16;
    g_nVisitedIndex = 0;
    g_fileType = XBinary::FT_UNKNOWN;
    g_disasmMode = XBinary::DM_UNKNOWN;
    g_nStartOffset = 0;
    g_nTotalSize = -1;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChangedSlot()));
    setXInfoDB(&g_emptyXInfoDB);
}

XDeviceTableView::~XDeviceTableView()
{
    // TODO Check
}

void XDeviceTableView::setXInfoDB(XInfoDB *pXInfoDB)
{
    if (g_pXInfoDB != pXInfoDB) {
        g_pXInfoDB = pXInfoDB;

        if (pXInfoDB) {
            connect(g_pXInfoDB, SIGNAL(reloadViewSignal()), this, SLOT(reloadView()));
        }
    }
}

XInfoDB *XDeviceTableView::getXInfoDB()
{
    return g_pXInfoDB;
}

void XDeviceTableView::setMode(XBinary::FT fileType, XBinary::DM disasmMode)
{
    g_fileType = fileType;
    g_disasmMode = disasmMode;

    g_memoryMap = XFormats::getMemoryMap(fileType, XBinary::MAPMODE_UNKNOWN, g_pDevice);
}

void XDeviceTableView::setDevice(QIODevice *pDevice, qint64 nStartOffset, qint64 nTotalSize)
{
    g_pDevice = pDevice;
    g_nStartOffset = nStartOffset;

    if (nTotalSize == -1) {
        if (pDevice) {
            g_nTotalSize = pDevice->size();
        }

    } else {
        g_nTotalSize = nTotalSize;
    }

    g_listVisited.clear();

    if (pDevice) {
        g_memoryMap = XFormats::getMemoryMap(XBinary::FT_BINARY, XBinary::MAPMODE_UNKNOWN, pDevice);

        XDeviceTableView::adjustScrollCount();
        //    setReadonly(!(pDevice->isWritable()));
        setActive(true);
    } else {
        setActive(false);
    }
}

QIODevice *XDeviceTableView::getDevice()
{
    return g_pDevice;
}

void XDeviceTableView::setViewSize(qint64 nViewSize)
{
    g_nViewSize = nViewSize;
}

qint64 XDeviceTableView::getViewSize()
{
    return g_nViewSize;
}

XBinary::_MEMORY_MAP *XDeviceTableView::getMemoryMap()
{
    return &g_memoryMap;
}

void XDeviceTableView::setLocationMode(XDeviceTableView::LOCMODE locationMode)
{
    g_locationMode = locationMode;

    adjust(true);
    viewport()->update();
    emit selectionChanged();
}

XDeviceTableView::LOCMODE XDeviceTableView::getlocationMode()
{
    return g_locationMode;
}

void XDeviceTableView::setLocationBase(qint32 nBase)
{
    g_nLocationBase = nBase;

    adjust(true);
    viewport()->update();
    emit selectionChanged();
}

qint32 XDeviceTableView::getLocationBase()
{
    return g_nLocationBase;
}

void XDeviceTableView::adjustScrollCount()
{
    if (g_pDevice) {
        setViewSize(g_pDevice->size());
    }
}

qint64 XDeviceTableView::getViewSizeByViewPos(qint64 nViewPos)
{
    Q_UNUSED(nViewPos)

    return 1;
}

qint64 XDeviceTableView::addressToViewPos(XADDR nAddress)
{
    qint64 nResult = 0;

    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);

    nResult = deviceOffsetToViewPos(nOffset);

    return nResult;
}

qint64 XDeviceTableView::locationToViewPos(XADDR nLocation, XBinary::LT locationType)
{
    qint64 nResult = 0;

    if (locationType == XBinary::LT_ADDRESS) {
        qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nLocation);
        nResult = deviceOffsetToViewPos(nOffset);
    } else if (locationType == XBinary::LT_RELADDRESS) {
        qint64 nOffset = XBinary::relAddressToOffset(getMemoryMap(), nLocation);
        nResult = deviceOffsetToViewPos(nOffset);
    } else if (locationType == XBinary::LT_OFFSET) {
        nResult = deviceOffsetToViewPos(nLocation);
    }

    return nResult;
}

XDeviceTableView::DEVICESTATE XDeviceTableView::getDeviceState(bool bGlobalOffset)
{
    DEVICESTATE result = {};
    STATE state = getState();

    result.nSelectionDeviceOffset = state.nSelectionViewPos;
    //    result.nCursorOffset = state.nCursorViewPos;
    result.nSelectionSize = state.nSelectionViewSize;
    result.nStartDeviceOffset = getViewPosStart();

    if (bGlobalOffset) {
        XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

        if (pSubDevice) {
            quint64 nInitOffset = pSubDevice->getInitLocation();
            result.nSelectionDeviceOffset += nInitOffset;
            //            result.nCursorOffset += nInitOffset;
            result.nStartDeviceOffset += nInitOffset;
        }
    }

    return result;
}

void XDeviceTableView::setDeviceState(const DEVICESTATE &deviceState, bool bGlobalOffset)
{
    DEVICESTATE _deviceState = deviceState;

    if (bGlobalOffset) {
        XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

        if (pSubDevice) {
            quint64 nInitOffset = pSubDevice->getInitLocation();
            _deviceState.nSelectionDeviceOffset -= nInitOffset;
            //            deviceState.nCursorOffset -= nInitOffset;
            _deviceState.nStartDeviceOffset -= nInitOffset;
        }
    }

    _goToViewPos(_deviceState.nStartDeviceOffset);
    _initSetSelection(_deviceState.nSelectionDeviceOffset, _deviceState.nSelectionSize);
    //    setCursorViewPos(deviceState.nCursorOffset);

    adjust();
    viewport()->update();
}

qint64 XDeviceTableView::deviceOffsetToViewPos(qint64 nOffset)
{
    qint64 nResult = 0;

    VIEWSTRUCT viewStruct = _getViewStructByOffset(nOffset);

    if (viewStruct.nSize) {
        nResult = viewStruct.nViewPos + (nOffset - viewStruct.nOffset);
    }

    return nResult;
}

qint64 XDeviceTableView::deviceSizeToViewSize(qint64 nOffset, qint64 nSize)
{
    Q_UNUSED(nOffset)

    qint64 nResult = nSize;

    return nResult;
}

qint64 XDeviceTableView::viewPosToDeviceOffset(qint64 nViewPos)
{
    qint64 nResult = -1;

    VIEWSTRUCT viewStruct = _getViewStructByViewPos(nViewPos);

    if (viewStruct.nSize && (viewStruct.nOffset != -1)) {
        nResult = viewStruct.nOffset + (nViewPos - viewStruct.nViewPos);
    }

    return nResult;
}

void XDeviceTableView::setDeviceSelection(qint64 nOffset, qint64 nSize)
{
    if (isSelectionEnable()) {
        qint64 nViewPos = deviceOffsetToViewPos(nOffset);

        _initSetSelection(nViewPos, nSize);

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
        qint64 nViewPos = g_listVisited.at(g_nVisitedIndex);

        if (_goToViewPos(nViewPos)) {
            _initSetSelection(nViewPos, getViewSizeByViewPos(nViewPos));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::goToPrevVisited()
{
    if (isPrevVisitedAvailable()) {
        g_nVisitedIndex--;
        qint64 nViewPos = g_listVisited.at(g_nVisitedIndex);

        if (_goToViewPos(nViewPos)) {
            _initSetSelection(nViewPos, getViewSizeByViewPos(nViewPos));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::addVisited(qint64 nViewPos)
{
    // #ifdef QT_DEBUG
    //     qDebug("Add visited %s", XBinary::valueToHex(nViewPos).toLatin1().data());
    // #endif

    if ((g_listVisited.empty()) || (g_listVisited.last() != nViewPos)) {
        qint32 nNumberOfVisited = g_listVisited.count();

        for (qint32 i = nNumberOfVisited - 1; i > g_nVisitedIndex; i--) {
            g_listVisited.removeAt(i);
        }

        g_listVisited.append(nViewPos);

        if (g_listVisited.count() > N_MAX_VISITED) {
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
        record.locationType = pList->at(i).locationType;
        record.nSize = pList->at(i).nSize;
        record.colBackground = pList->at(i).colBackground;
        record.colBackgroundSelected = getColorSelected(record.colBackground);
        record.sComment = pList->at(i).sComment;

        listResult.append(record);
    }

    return listResult;
}

QList<XDeviceTableView::HIGHLIGHTREGION> XDeviceTableView::getHighlightRegion(QList<HIGHLIGHTREGION> *pList, quint64 nLocation, XBinary::LT locationType)
{
    QList<HIGHLIGHTREGION> listResult;

    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pList->at(i).locationType == locationType) {
            if ((nLocation >= pList->at(i).nLocation) && (nLocation < (pList->at(i).nLocation + pList->at(i).nSize))) {
                listResult.append(pList->at(i));
            }
        }
    }

    return listResult;
}

void XDeviceTableView::setViewWidgetState(VIEWWIDGET viewWidget, bool bState)
{
    if (bState) {
        g_stViewWidgetState.insert(viewWidget);
    } else {
        g_stViewWidgetState.remove(viewWidget);
    }

    emit viewWidgetsStateChanged();
}

bool XDeviceTableView::getViewWidgetState(VIEWWIDGET viewWidget)
{
    return g_stViewWidgetState.contains(viewWidget);
}

void XDeviceTableView::dumpMemory(const QString &sDumpName, qint64 nOffset, qint64 nSize)
{
    QString sSaveFileName = XBinary::getResultFileName(getDevice(), QString("%1.bin").arg(sDumpName));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.bin)").arg(tr("Raw data")));

    if (!sFileName.isEmpty()) {
        DialogDumpProcess dd(this);
        dd.setGlobal(getShortcuts(), getGlobalOptions());
        dd.setData(getDevice(), nOffset, nSize, sFileName, DumpProcess::DT_DUMP_DEVICE_OFFSET);

        dd.showDialogDelay();
    }
}

void XDeviceTableView::setLocation(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    goToLocation(nLocation, (XBinary::LT)nLocationType);
    setLocationOffset(nLocation, (XBinary::LT)nLocationType, nSize);
}

XDeviceTableView::VIEWSTRUCT XDeviceTableView::_getViewStructByViewPos(qint64 nViewPos)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = g_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((g_listViewStruct.at(i).nViewPos <= nViewPos) && (nViewPos < (g_listViewStruct.at(i).nViewPos + g_listViewStruct.at(i).nSize))) {
            result = g_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XDeviceTableView::VIEWSTRUCT XDeviceTableView::_getViewStructByAddress(XADDR nAddress)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = g_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((g_listViewStruct.at(i).nAddress != (XADDR)-1) && (g_listViewStruct.at(i).nAddress <= nAddress) &&
            (nAddress < (g_listViewStruct.at(i).nAddress + g_listViewStruct.at(i).nSize))) {
            result = g_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XDeviceTableView::VIEWSTRUCT XDeviceTableView::_getViewStructByOffset(qint64 nOffset)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = g_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((g_listViewStruct.at(i).nOffset != -1) && (g_listViewStruct.at(i).nOffset <= nOffset) &&
            (nOffset < (g_listViewStruct.at(i).nOffset + g_listViewStruct.at(i).nSize))) {
            result = g_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

qint64 XDeviceTableView::write_array(qint64 nOffset, char *pData, qint64 nDataSize)
{
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
    QByteArray baResult;

    if (getDevice()) {
        baResult = XBinary::read_array(getDevice(), nOffset, nSize);
    }

    return baResult;
}

void XDeviceTableView::goToAddress(XADDR nAddress, bool bShort, bool bAprox, bool bSaveVisited)
{
    goToLocation(nAddress, XBinary::LT_ADDRESS, bShort, bAprox, bSaveVisited);
}

void XDeviceTableView::goToOffset(qint64 nOffset, bool bShort, bool bAprox, bool bSaveVisited)
{
    goToLocation(nOffset, XBinary::LT_OFFSET, bShort, bAprox, bSaveVisited);
}

void XDeviceTableView::goToLocation(XADDR nLocation, XBinary::LT locationType, bool bShort, bool bAprox, bool bSaveVisited)
{
    if (nLocation != (XADDR)-1) {
        qint64 nViewPos = locationToViewPos(nLocation, locationType);

        if (bSaveVisited) {
            addVisited(getState().nSelectionViewPos);
        }

        if (_goToViewPos(nViewPos, false, bShort, bAprox)) {
            if (bSaveVisited) {
                addVisited(nViewPos);
            }

            qint64 nViewSize = getViewSizeByViewPos(nViewPos);

            _initSetSelection(nViewPos, nViewSize);
            // TODO
        }
        // mb TODO reload
    }
}

void XDeviceTableView::setLocationOffset(XADDR nLocation, XBinary::LT locationType, qint64 nSize)
{
    qint64 nOffset = 0;

    // TODO move to Binary
    if (locationType == XBinary::LT_ADDRESS) {
        nOffset = XBinary::addressToOffset(getMemoryMap(), nLocation);
    } else if (locationType == XBinary::LT_RELADDRESS) {
        nOffset = XBinary::relAddressToOffset(getMemoryMap(), nLocation);
    } else if (locationType == XBinary::LT_OFFSET) {
        nOffset = nLocation;
    }

    setDeviceSelection(nOffset, nSize);
}

void XDeviceTableView::setSelectionAddress(XADDR nAddress, qint64 nSize)
{
    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);

    setSelectionOffset(nOffset, nSize);
}

void XDeviceTableView::setSelectionRelAddress(XADDR nRelAddress, qint64 nSize)
{
    qint64 nOffset = XBinary::relAddressToOffset(getMemoryMap(), nRelAddress);

    setSelectionOffset(nOffset, nSize);
}

void XDeviceTableView::setSelectionOffset(qint64 nOffset, qint64 nSize)
{
    if (nOffset != -1) {
        qint64 nViewPos = deviceOffsetToViewPos(nOffset);
        qint64 nViewSize = deviceSizeToViewSize(nOffset, nSize);

        _initSetSelection(nViewPos, nViewSize);
        viewport()->update();  // TODO Check
    }
}

bool XDeviceTableView::isEdited()
{
    bool bResult = XBinary::isBackupPresent(XBinary::getBackupDevice(getDevice()));

    return bResult;
}

bool XDeviceTableView::saveBackup()
{
    bool bResult = true;

    if ((getGlobalOptions()->isSaveBackup()) && (!isEdited())) {
        // Save backup
        bResult = XBinary::saveBackup(XBinary::getBackupDevice(getDevice()));
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

void XDeviceTableView::adjustAfterAnalysis()
{
    adjustScrollCount();
    clearVisited();

    reload(true);
}

bool XDeviceTableView::isViewPosValid(qint64 nViewPos)
{
    bool bResult = false;

    if ((nViewPos >= 0) && (nViewPos < g_nViewSize)) {
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
    DEVICESTATE state = getDeviceState();
    XADDR nAddress = 0;

    DialogGoToAddress::TYPE type = DialogGoToAddress::TYPE_VIRTUALADDRESS;

    if (g_locationMode == LOCMODE_RELADDRESS) {
        nAddress = XBinary::offsetToRelAddress(getMemoryMap(), state.nSelectionDeviceOffset);
    } else {
        nAddress = XBinary::offsetToAddress(getMemoryMap(), state.nSelectionDeviceOffset);
    }

    DialogGoToAddress da(this, getMemoryMap(), type, nAddress);
    da.setGlobal(getShortcuts(), getGlobalOptions());

    if (da.exec() == QDialog::Accepted)  // TODO use status
    {
        goToAddress(da.getValue(), false, false, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToOffsetSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogGoToAddress da(this, getMemoryMap(), DialogGoToAddress::TYPE_OFFSET, state.nSelectionDeviceOffset);
    da.setGlobal(getShortcuts(), getGlobalOptions());

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
        goToOffset(state.nSelectionDeviceOffset, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToSelectionEnd()
{
    DEVICESTATE state = getDeviceState();

    if (state.nSelectionSize) {
        qint64 nOffset = state.nSelectionDeviceOffset + state.nSelectionSize;

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
    DEVICESTATE state = getDeviceState();

    dumpMemory(tr("Dump"), state.nSelectionDeviceOffset, state.nSelectionSize);
}

void XDeviceTableView::_hexSignatureSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogHexSignature dhs(this, getDevice(), state.nSelectionDeviceOffset, state.nSelectionSize);
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
    g_searchData.nCurrentOffset = state.nSelectionDeviceOffset;

    DialogSearch::OPTIONS options = {};
    options.bShowBegin = true;

    DialogSearch dialogSearch(this, getDevice(), &g_searchData, mode, options);

    if (dialogSearch.exec() == QDialog::Accepted)  // TODO use status
    {
        DialogSearchProcess dsp(this, getDevice(), &g_searchData);
        dsp.setGlobal(getShortcuts(), getGlobalOptions());

        dsp.showDialogDelay();

        if (g_searchData.nResultOffset != -1) {
            qint64 nViewPos = deviceOffsetToViewPos(g_searchData.nResultOffset);
            qint64 nViewSize = deviceSizeToViewSize(g_searchData.nResultOffset, g_searchData.nResultSize);

            _goToViewPos(nViewPos);
            _initSetSelection(nViewPos, nViewSize);
            setFocus();
            viewport()->update();

        } else {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_findNextSlot()
{
    if (g_searchData.bIsInit) {
        g_searchData.nCurrentOffset = g_searchData.nResultOffset + 1;
        g_searchData.startFrom = XBinary::SF_CURRENTOFFSET;

        DialogSearchProcess dialogSearch(this, getDevice(), &g_searchData);
        dialogSearch.setGlobal(getShortcuts(), getGlobalOptions());

        dialogSearch.showDialogDelay();

        if (dialogSearch.isSuccess())  // TODO use status
        {
            qint64 nViewPos = deviceOffsetToViewPos(g_searchData.nResultOffset);
            qint64 nViewSize = deviceSizeToViewSize(g_searchData.nResultOffset, g_searchData.nResultSize);

            _goToViewPos(nViewPos);
            _initSetSelection(nViewPos, nViewSize);
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

    DialogShowData dialogShowData(this, getDevice(), state.nSelectionDeviceOffset, state.nSelectionSize);
    dialogShowData.setGlobal(getShortcuts(), getGlobalOptions());

    dialogShowData.exec();
}

void XDeviceTableView::_copyAddressSlot()
{
    DEVICESTATE state = getDeviceState();

    XADDR nAddress = XBinary::offsetToAddress(getMemoryMap(), state.nSelectionDeviceOffset);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyRelAddressSlot()
{
    DEVICESTATE state = getDeviceState();

    XADDR nAddress = XBinary::offsetToRelAddress(getMemoryMap(), state.nSelectionDeviceOffset);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyOffsetSlot()
{
    DEVICESTATE state = getDeviceState();

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, state.nSelectionDeviceOffset));
}

void XDeviceTableView::_setEdited(qint64 nDeviceOffset, qint64 nDeviceSize)
{
    setEdited(nDeviceOffset, nDeviceSize);

    emit dataChanged(nDeviceOffset, nDeviceSize);  // TODO initOffset
}

void XDeviceTableView::goToAddressSlot(XADDR nAddress, qint64 nSize)
{
    goToAddress(nAddress, true, true, true);

    if (nSize) {
        setSelectionAddress(nAddress, nSize);
    }

    reload(true);
}

void XDeviceTableView::goToAddressSlot(XADDR nAddress)
{
    goToAddressSlot(nAddress, 0);
}

void XDeviceTableView::reloadView()
{
    updateData();

    viewport()->update();
}

void XDeviceTableView::selectionChangedSlot()
{
    XDeviceTableView::DEVICESTATE deviceState = getDeviceState(true);

    emit currentLocationChanged(deviceState.nSelectionDeviceOffset, XBinary::LT_OFFSET, deviceState.nSelectionSize);
}

void XDeviceTableView::changeLocationMode()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        LOCMODE mode = (LOCMODE)pAction->property("mode").toUInt();

        setLocationMode(mode);
    }
}

void XDeviceTableView::changeLocationBase()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        qint32 nBase = (LOCMODE)pAction->property("base").toInt();

        setLocationBase(nBase);
    }
}

void XDeviceTableView::_bookmarkList()
{
    if (getXInfoDB()) {
        if (!getViewWidgetState(VIEWWIDGET_BOOKMARKS)) {
            setViewWidgetState(VIEWWIDGET_BOOKMARKS, true);

            quint64 nLocation = 0;
            XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

            if (pSubDevice) {
                nLocation = pSubDevice->getInitLocation();
            }

            DialogBookmarks dialogBookmarks(this);

            dialogBookmarks.setData(getXInfoDB(), nLocation, -1, getDevice()->size());

            connect(&dialogBookmarks, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));
            connect(this, SIGNAL(closeWidget_Bookmarks()), &dialogBookmarks, SLOT(close()));

            XOptions::_adjustStayOnTop(&dialogBookmarks, true);
            dialogBookmarks.exec();

            setViewWidgetState(VIEWWIDGET_BOOKMARKS, false);
        } else {
            emit closeWidget_Bookmarks();
        }
    }
}
void XDeviceTableView::_bookmarkNew()
{
    if (getXInfoDB()) {
        DEVICESTATE state = getDeviceState(true);

        QString sComment =
            QString("%1 - %2").arg(QString::number(state.nSelectionDeviceOffset, 16), QString::number(state.nSelectionDeviceOffset + state.nSelectionSize, 16));

        XInfoDB::BOOKMARKRECORD record = {};
        record.colBackground = QColor(Qt::yellow);
        record.nLocation = state.nSelectionDeviceOffset;
        record.locationType = XBinary::LT_OFFSET;
        record.nSize = state.nSelectionSize;
        record.sComment = sComment;

        getXInfoDB()->_addBookmarkRecord(record);

        getXInfoDB()->reloadView();
    }
}

void XDeviceTableView::currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    setLocation(nLocation, nLocationType, nSize);

    reload(true);
}

void XDeviceTableView::_showDataInspector()
{
    if (!getViewWidgetState(VIEWWIDGET_DATAINSPECTOR)) {
        setViewWidgetState(VIEWWIDGET_DATAINSPECTOR, true);

        XDeviceTableView::DEVICESTATE deviceState = getDeviceState();

        DialogDataInspector dialogDataInspector(this, getDevice(), deviceState.nSelectionDeviceOffset, deviceState.nSelectionSize);
        dialogDataInspector.setGlobal(getShortcuts(), getGlobalOptions());

        connect(this, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), &dialogDataInspector, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));
        connect(this, SIGNAL(dataChanged(qint64, qint64)), &dialogDataInspector, SLOT(dataChangedSlot(qint64, qint64)));
        connect(&dialogDataInspector, SIGNAL(dataChanged(qint64, qint64)), this, SLOT(_setEdited(qint64, qint64)));
        connect(this, SIGNAL(closeWidget_DataInspector()), &dialogDataInspector, SLOT(close()));

        XOptions::_adjustStayOnTop(&dialogDataInspector, true);

        dialogDataInspector.exec();

        setViewWidgetState(VIEWWIDGET_DATAINSPECTOR, false);
    } else {
        emit closeWidget_DataInspector();
    }
}

void XDeviceTableView::_showDataConvertor()
{
    if (!getViewWidgetState(VIEWWIDGET_DATACONVERTOR)) {
        setViewWidgetState(VIEWWIDGET_DATACONVERTOR, true);

        XDeviceTableView::DEVICESTATE deviceState = getDeviceState();

        SubDevice sd(getDevice(), deviceState.nSelectionDeviceOffset, deviceState.nSelectionSize);

        if (sd.open(QIODevice::ReadOnly)) {
            DialogXDataConvertor dialogDataConvertor(this);
            dialogDataConvertor.setGlobal(getShortcuts(), getGlobalOptions());
            dialogDataConvertor.setData(&sd);
            connect(this, SIGNAL(closeWidget_DataConvertor()), &dialogDataConvertor, SLOT(close()));

            dialogDataConvertor.exec();

            setViewWidgetState(VIEWWIDGET_DATACONVERTOR, false);

            sd.close();
        }
    } else {
        emit closeWidget_DataConvertor();
    }
}

void XDeviceTableView::_showMultisearch()
{
    if (!getViewWidgetState(VIEWWIDGET_MULTISEARCH)) {
        setViewWidgetState(VIEWWIDGET_MULTISEARCH, true);

        SearchValuesWidget::OPTIONS options = {};
        //        options.fileType = XBinary::FT_REGION;
        options.fileType = XBinary::FT_UNKNOWN;

        DialogSearchValues dialogSearchValues(this);
        dialogSearchValues.setGlobal(getShortcuts(), getGlobalOptions());
        dialogSearchValues.setData(getDevice(), options);

        connect(&dialogSearchValues, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));
        connect(this, SIGNAL(closeWidget_Multisearch()), &dialogSearchValues, SLOT(close()));

        XOptions::_adjustStayOnTop(&dialogSearchValues, true);

        dialogSearchValues.exec();

        setViewWidgetState(VIEWWIDGET_MULTISEARCH, false);
    } else {
        emit closeWidget_Multisearch();
    }
}
