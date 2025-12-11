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
    m_pXInfoDB = nullptr;
    m_nViewSize = 0;
    m_searchData = {};
    m_locationMode = LOCMODE_ADDRESS;
    m_nLocationBase = 16;
    m_nVisitedIndex = 0;
    m_fileType = XBinary::FT_UNKNOWN;
    m_disasmMode = XBinary::DM_UNKNOWN;
    m_nStartOffset = 0;
    m_nTotalSize = -1;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChangedSlot()));
    setXInfoDB(&m_emptyXInfoDB);
}

XDeviceTableView::~XDeviceTableView()
{
    // TODO Check
}

void XDeviceTableView::setXInfoDB(XInfoDB *pXInfoDB)
{
    if (m_pXInfoDB != pXInfoDB) {
        m_pXInfoDB = pXInfoDB;

        if (pXInfoDB) {
            connect(m_pXInfoDB, SIGNAL(reloadViewSignal()), this, SLOT(reloadView()));
        }
    }
}

XInfoDB *XDeviceTableView::getXInfoDB()
{
    return m_pXInfoDB;
}

void XDeviceTableView::setMode(XBinary::FT fileType, XBinary::DM disasmMode, bool bShowVirtual)
{
    m_fileType = fileType;
    m_disasmMode = disasmMode;

    m_disasmCore.setMode(disasmMode);

    m_memoryMap = XFormats::getMemoryMap(fileType, XBinary::MAPMODE_UNKNOWN, m_binaryView.getDevice());

    XVPOS nViewPos = 0;

    bool bAll = false;

    if (m_binaryView.getDevice()) {
        if ((m_nStartOffset == 0) && (m_nTotalSize == m_binaryView.getDevice()->size())) {
            bAll = true;
        }
    }

    m_listViewStruct.clear();

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

        // Add if m_nStartOffset and qint64 m_nTotalSize in this viewStruct
        if (!bAll) {
            if (record.nOffset != -1) {
                if (record.nOffset >= m_nStartOffset && (record.nOffset + record.nSize <= m_nStartOffset + m_nTotalSize)) {
                    bAdd = true;
                } else {
                    bAdd = false;
                }
            }
        }

        // Add if m_nStartOffset and qint64 m_nTotalSize partially in this viewStruct, correct nAddress, nOffset, nSize
        if (!bAll) {
            if (record.nOffset != -1) {
                if (record.nOffset < m_nStartOffset && (record.nOffset + record.nSize > m_nStartOffset)) {
                    record.nAddress += m_nStartOffset - record.nOffset;
                    record.nOffset = m_nStartOffset;
                    record.nSize -= m_nStartOffset - record.nOffset;
                    bAdd = true;
                }
                if (record.nOffset < m_nStartOffset + m_nTotalSize && (record.nOffset + record.nSize > m_nStartOffset + m_nTotalSize)) {
                    record.nSize = m_nStartOffset + m_nTotalSize - record.nOffset;
                    bAdd = true;
                }
            }
        }

        if (bAdd) {
            nViewPos += record.nSize;

            m_listViewStruct.append(record);
        }
    }

    setViewSize(nViewPos);
}

void XDeviceTableView::setDevice(QIODevice *pDevice, qint64 nStartOffset, qint64 nTotalSize)
{
    m_binaryView.setData(XBinary::FT_BINARY, pDevice, false, -1);
    m_nStartOffset = nStartOffset;

    m_nTotalSize = nTotalSize;

    if ((nTotalSize == -1) || (nTotalSize == 0)) {
        if (pDevice) {
            m_nTotalSize = pDevice->size() - nStartOffset;
        } else {
            m_nTotalSize = 0;
        }
    }

    if (pDevice) {
        if (nStartOffset + nTotalSize > pDevice->size()) {
            m_nTotalSize = pDevice->size() - nStartOffset;
        }
    }

    m_listVisited.clear();

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
    return m_binaryView.getDevice();
}

void XDeviceTableView::setViewSize(qint64 nViewSize)
{
    m_nViewSize = nViewSize;
}

qint64 XDeviceTableView::getViewSize()
{
    return m_nViewSize;
}

XBinary::_MEMORY_MAP *XDeviceTableView::getMemoryMap()
{
    return &m_memoryMap;
}

void XDeviceTableView::setLocationMode(XDeviceTableView::LOCMODE locationMode)
{
    m_locationMode = locationMode;

    adjust(true);
    viewport()->update();
    emit selectionChanged();
}

XDeviceTableView::LOCMODE XDeviceTableView::getlocationMode()
{
    return m_locationMode;
}

void XDeviceTableView::setLocationBase(qint32 nBase)
{
    m_nLocationBase = nBase;

    adjust(true);
    viewport()->update();
    emit selectionChanged();
}

qint32 XDeviceTableView::getLocationBase()
{
    return m_nLocationBase;
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

XVPOS XDeviceTableView::deviceOffsetToViewPos(qint64 nOffset)
{
    XVPOS nResult = 0;

    VIEWSTRUCT viewStruct = _getViewStructByOffset(nOffset);

    if (viewStruct.nSize) {
        nResult = viewStruct.nViewPos + (nOffset - viewStruct.nOffset);
    }

    return nResult;
}

qint64 XDeviceTableView::viewPosToDeviceOffset(XVPOS nViewPos)
{
    qint64 nResult = -1;

    VIEWSTRUCT viewStruct = _getViewStructByViewPos(nViewPos);

    if (viewStruct.nSize && (viewStruct.nOffset != -1)) {
        nResult = viewStruct.nOffset + (nViewPos - viewStruct.nViewPos);
    }

    return nResult;
}

XADDR XDeviceTableView::viewPosToAddress(XVPOS nViewPos)
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
    return m_nVisitedIndex > 0;
}

bool XDeviceTableView::isNextVisitedAvailable()
{
    return m_nVisitedIndex < (m_listVisited.count() - 1);
}

void XDeviceTableView::goToNextVisited()
{
    if (isNextVisitedAvailable()) {
        m_nVisitedIndex++;
        qint64 nViewPos = m_listVisited.at(m_nVisitedIndex);

        if (_goToViewPos(nViewPos)) {
            _initSetSelection(nViewPos, getViewSizeByViewPos(nViewPos));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::goToPrevVisited()
{
    if (isPrevVisitedAvailable()) {
        m_nVisitedIndex--;
        qint64 nViewPos = m_listVisited.at(m_nVisitedIndex);

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

    if ((m_listVisited.empty()) || (m_listVisited.last() != nViewPos)) {
        qint32 nNumberOfVisited = m_listVisited.count();

        for (qint32 i = nNumberOfVisited - 1; i > m_nVisitedIndex; i--) {
            m_listVisited.removeAt(i);
        }

        m_listVisited.append(nViewPos);

        if (m_listVisited.count() > N_MAX_VISITED) {
            m_listVisited.removeFirst();
        }

        m_nVisitedIndex = m_listVisited.count() - 1;

        emit visitedStateChanged();
    }
}

void XDeviceTableView::clearVisited()
{
    m_listVisited.clear();
}

void XDeviceTableView::setLocation(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    goToLocation(nLocation, (XBinary::LT)nLocationType);
    setLocationOffset(nLocation, (XBinary::LT)nLocationType, nSize);
}

XDisasmCore *XDeviceTableView::getDisasmCore()
{
    return &m_disasmCore;
}

XDeviceTableView::VIEWSTRUCT XDeviceTableView::_getViewStructByViewPos(XVPOS nViewPos)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = m_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((m_listViewStruct.at(i).nViewPos <= nViewPos) && (nViewPos < (m_listViewStruct.at(i).nViewPos + m_listViewStruct.at(i).nSize))) {
            result = m_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XDeviceTableView::VIEWSTRUCT XDeviceTableView::_getViewStructByAddress(XADDR nAddress)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = m_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((m_listViewStruct.at(i).nAddress != (XADDR)-1) && (m_listViewStruct.at(i).nAddress <= nAddress) &&
            (nAddress < (m_listViewStruct.at(i).nAddress + m_listViewStruct.at(i).nSize))) {
            result = m_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XDeviceTableView::VIEWSTRUCT XDeviceTableView::_getViewStructByOffset(qint64 nOffset)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = m_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((m_listViewStruct.at(i).nOffset != -1) && (m_listViewStruct.at(i).nOffset <= nOffset) &&
            (nOffset < (m_listViewStruct.at(i).nOffset + m_listViewStruct.at(i).nSize))) {
            result = m_listViewStruct.at(i);
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

    if (m_pXInfoDB) {
        m_pXInfoDB->setEdited(nDeviceOffset, nDeviceSize);
    }

    viewport()->update();
}

void XDeviceTableView::adjustAfterAnalysis()
{
    adjustScrollCount();
    clearVisited();

    reload(true);
}

bool XDeviceTableView::isViewPosValid(XVPOS nViewPos)
{
    bool bResult = false;

    if ((nViewPos >= 0) && (nViewPos < m_nViewSize)) {
        bResult = true;
    }

    return bResult;
}

bool XDeviceTableView::isEnd(XVPOS nOffset)
{
    return (nOffset == m_nViewSize);
}

void XDeviceTableView::_goToAddressSlot()
{
    DEVICESTATE state = getDeviceState();
    XADDR nAddress = 0;

    DialogGoToAddress::TYPE type = DialogGoToAddress::TYPE_VIRTUALADDRESS;

    if (m_locationMode == LOCMODE_RELADDRESS) {
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
    _findSlot(XBinary::SEARCHMODE_STRING);
}

void XDeviceTableView::_findSignatureSlot()
{
    _findSlot(XBinary::SEARCHMODE_SIGNATURE);
}

void XDeviceTableView::_findValueSlot()
{
    _findSlot(XBinary::SEARCHMODE_VALUE);
}

void XDeviceTableView::_findSlot(XBinary::SEARCHMODE mode)
{
    DEVICESTATE state = getDeviceState();

    m_searchData = {};
    m_searchData.nResultOffset = -1;
    m_searchData.nCurrentOffset = state.nSelectionDeviceOffset;

    DialogSearch::OPTIONS options = {};
    options.bShowBegin = true;

    DialogSearch dialogSearch(this, getDevice(), &m_searchData, mode, options);

    if (dialogSearch.exec() == QDialog::Accepted)  // TODO use status
    {
        SearchProcess searchProcess;
        XDialogProcess dsp(this, &searchProcess);
        dsp.setGlobal(getShortcuts(), getGlobalOptions());
        searchProcess.setData(getDevice(), &m_searchData, dsp.getPdStruct());
        dsp.start();
        dsp.showDialogDelay();

        if (m_searchData.nResultOffset != -1) {
            qint64 nViewPos = deviceOffsetToViewPos(m_searchData.nResultOffset);
            qint64 nViewSize = m_searchData.nResultSize;

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
    if (m_searchData.bIsInit) {
        m_searchData.nCurrentOffset = m_searchData.nResultOffset + 1;
        m_searchData.startFrom = XBinary::SF_CURRENTOFFSET;

        SearchProcess searchProcess;
        XDialogProcess dsp(this, &searchProcess);
        dsp.setGlobal(getShortcuts(), getGlobalOptions());
        searchProcess.setData(getDevice(), &m_searchData, dsp.getPdStruct());
        dsp.start();
        dsp.showDialogDelay();

        if (dsp.isSuccess())  // TODO use status
        {
            qint64 nViewPos = deviceOffsetToViewPos(m_searchData.nResultOffset);
            qint64 nViewSize = m_searchData.nResultSize;

            _goToViewPos(nViewPos);
            _initSetSelection(nViewPos, nViewSize);
            setFocus();
            viewport()->update();
        } else if (m_searchData.valueType != XBinary::VT_UNKNOWN) {
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
