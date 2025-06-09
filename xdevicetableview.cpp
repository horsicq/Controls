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

void XDeviceTableView::setMode(XBinary::FT fileType, XBinary::DM disasmMode, bool bShowVirtual)
{
    g_fileType = fileType;
    g_disasmMode = disasmMode;

    g_disasmCore.setMode(disasmMode);

    g_memoryMap = XFormats::getMemoryMap(fileType, XBinary::MAPMODE_UNKNOWN, g_pDevice);

    qint64 nViewPos = 0;

    bool bAll = false;

    if (g_pDevice) {
        if ((g_nStartOffset == 0) && (g_nTotalSize == g_pDevice->size())) {
            bAll = true;
        }
    }

    g_listViewStruct.clear();

    qint32 nNumberOfRecords = getMemoryMap()->listRecords.count();
    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        VIEWSTRUCT record = {};
        record.nAddress = getMemoryMap()->listRecords.at(i).nAddress;
        record.nOffset = getMemoryMap()->listRecords.at(i).nOffset;
        record.nSize = getMemoryMap()->listRecords.at(i).nSize;
        // record.nScrollStart = nScrollStart;
        record.nViewPos = nViewPos;
        // record.nScrollCount = record.nSize;

        bool bAdd = true;
        // // TODO XInfoDB

        if (getMemoryMap()->listRecords.at(i).bIsInvisible) {
            bAdd = false;
        }

        if (!bShowVirtual) {
            if (getMemoryMap()->listRecords.at(i).bIsVirtual) {
                bAdd = false;
            }
        }

        // Add if g_nStartOffset and qint64 g_nTotalSize in this viewStruct
        if (!bAll) {
            if (record.nOffset != -1) {
                if (record.nOffset >= g_nStartOffset && (record.nOffset + record.nSize <= g_nStartOffset + g_nTotalSize)) {
                    bAdd = true;
                } else {
                    bAdd = false;
                }
            }
        }

        // Add if g_nStartOffset and qint64 g_nTotalSize partially in this viewStruct, correct nAddress, nOffset, nSize
        if (!bAll) {
            if (record.nOffset != -1) {
                if (record.nOffset < g_nStartOffset && (record.nOffset + record.nSize > g_nStartOffset)) {
                    record.nAddress += g_nStartOffset - record.nOffset;
                    record.nOffset = g_nStartOffset;
                    record.nSize -= g_nStartOffset - record.nOffset;
                    bAdd = true;
                }
                if (record.nOffset < g_nStartOffset + g_nTotalSize && (record.nOffset + record.nSize > g_nStartOffset + g_nTotalSize)) {
                    record.nSize = g_nStartOffset + g_nTotalSize - record.nOffset;
                    bAdd = true;
                }
            }
        }

        if (bAdd) {
            nViewPos += record.nSize;

            g_listViewStruct.append(record);
        }
    }

    setViewSize(nViewPos);
}

void XDeviceTableView::setDevice(QIODevice *pDevice, qint64 nStartOffset, qint64 nTotalSize)
{
    g_pDevice = pDevice;
    g_nStartOffset = nStartOffset;

    g_nTotalSize = nTotalSize;

    if ((nTotalSize == -1) || (nTotalSize == 0)) {
        if (pDevice) {
            g_nTotalSize = pDevice->size() - nStartOffset;
        } else {
            g_nTotalSize = 0;
        }
    }

    if (pDevice) {
        if (nStartOffset + nTotalSize > pDevice->size()) {
            g_nTotalSize = pDevice->size() - nStartOffset;
        }
    }

    g_listVisited.clear();

    if (pDevice) {
        setMode(XBinary::FT_BINARY, XBinary::DM_UNKNOWN, false);

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
    // TODO
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

XDeviceTableView::DEVICESTATE XDeviceTableView::getDeviceState()
{
    DEVICESTATE result = {};
    STATE state = getState();

    result.nSelectionDeviceOffset = viewPosToDeviceOffset(state.nSelectionViewPos);
    //    result.nCursorOffset = state.nCursorViewPos;
    result.nSelectionSize = state.nSelectionViewSize;
    result.nStartDeviceOffset = viewPosToDeviceOffset(getViewPosStart());

    return result;
}

void XDeviceTableView::setDeviceState(const DEVICESTATE &deviceState)
{
    _goToViewPos(deviceOffsetToViewPos(deviceState.nStartDeviceOffset));
    _initSetSelection(deviceOffsetToViewPos(deviceState.nSelectionDeviceOffset), deviceState.nSelectionSize);
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

qint64 XDeviceTableView::viewPosToDeviceOffset(qint64 nViewPos)
{
    qint64 nResult = -1;

    VIEWSTRUCT viewStruct = _getViewStructByViewPos(nViewPos);

    if (viewStruct.nSize && (viewStruct.nOffset != -1)) {
        nResult = viewStruct.nOffset + (nViewPos - viewStruct.nViewPos);
    }

    return nResult;
}

XADDR XDeviceTableView::viewPosToAddress(qint64 nViewPos)
{
    XADDR nResult = -1;

    VIEWSTRUCT viewStruct = _getViewStructByViewPos(nViewPos);

    if (viewStruct.nSize && (viewStruct.nOffset != -1)) {
        nResult = viewStruct.nAddress + (nViewPos - viewStruct.nViewPos);
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

void XDeviceTableView::setLocation(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    goToLocation(nLocation, (XBinary::LT)nLocationType);
    setLocationOffset(nLocation, (XBinary::LT)nLocationType, nSize);
}

XDisasmCore *XDeviceTableView::getDisasmCore()
{
    return &g_disasmCore;
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
        qint64 nViewSize = nSize;

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
            qint64 nViewSize = g_searchData.nResultSize;

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
            qint64 nViewSize = g_searchData.nResultSize;

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
    _initSetSelection(0, getViewSize());
    viewport()->update();
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
    XDeviceTableView::DEVICESTATE deviceState = getDeviceState();

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

void XDeviceTableView::currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    setLocation(nLocation, nLocationType, nSize);

    reload(true);
}
