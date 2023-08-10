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
#include "xdevicetableeditview.h"

XDeviceTableEditView::XDeviceTableEditView(QWidget *pParent) : XDeviceTableView(pParent)
{
}

void XDeviceTableEditView::_editHex()
{
    if (!isReadonly()) {
        DEVICESTATE state = getDeviceState();

        SubDevice sd(getDevice(), state.nSelectionDeviceOffset, state.nSelectionSize);

        if (sd.open(QIODevice::ReadWrite)) {
            DialogHexEdit dialogHexEdit(this);
            dialogHexEdit.setGlobal(getShortcuts(), getGlobalOptions());

            //        connect(&dialogHexEdit,SIGNAL(changed()),this,SLOT(_setEdited()));

            dialogHexEdit.setData(&sd, state.nSelectionDeviceOffset);
            dialogHexEdit.setBackupDevice(getBackupDevice());

            dialogHexEdit.exec();

            _setEdited(state.nSelectionDeviceOffset, state.nSelectionSize);

            sd.close();
        }
    }
}

void XDeviceTableEditView::_followInDisasmSlot()
{
    quint64 nOffset = getDeviceState(true).nSelectionDeviceOffset;
    //    XADDR nAddress=XBinary::offsetToAddress(getMemoryMap(),nOffset);

    emit followInDisasm(nOffset);
}

void XDeviceTableEditView::_followInHexSlot()
{
    //    emit
    //    followInHex(XBinary::offsetToAddress(getMemoryMap(),getStateOffset()));
    quint64 nOffset = getDeviceState(true).nSelectionDeviceOffset;
    //    XADDR nAddress=XBinary::offsetToAddress(getMemoryMap(),nOffset);

    emit followInHex(nOffset);
}
#ifdef QT_SQL_LIB
void XDeviceTableEditView::_bookmarkList()
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

            connect(&dialogBookmarks, SIGNAL(currentBookmarkChanged(quint64, qint32, qint64)), this, SLOT(currentBookmarkChangedSlot(quint64, qint32, qint64)));
            connect(this, SIGNAL(closeWidget_Bookmarks()), &dialogBookmarks, SLOT(close()));

            XOptions::_adjustStayOnTop(&dialogBookmarks, true);
            dialogBookmarks.exec();

            setViewWidgetState(VIEWWIDGET_BOOKMARKS, false);
        } else {
            emit closeWidget_Bookmarks();
        }
    }
}
#endif
#ifdef QT_SQL_LIB
void XDeviceTableEditView::currentBookmarkChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    Q_UNUSED(nSize)

    if (nLocationType == XInfoDB::LT_ADDRESS) {
        goToAddressSlot(nLocation);
        viewport()->update();
    } else if (nLocationType == XInfoDB::LT_OFFSET) {
        goToOffset(nLocation);
        viewport()->update();
    }
}
#endif
#ifdef QT_SQL_LIB
void XDeviceTableEditView::_bookmarkNew()
{
    if (getXInfoDB()) {
        DEVICESTATE state = getDeviceState(true);

        QString sComment =
            QString("%1 - %2").arg(QString::number(state.nSelectionDeviceOffset, 16), QString::number(state.nSelectionDeviceOffset + state.nSelectionSize, 16));

        getXInfoDB()->_addBookmarkRecord(state.nSelectionDeviceOffset, XInfoDB::LT_OFFSET, state.nSelectionSize, QColor(Qt::yellow),
                                         sComment);  // mb TODO Colors TODO locationType

        getXInfoDB()->reloadView();
    }
}
#endif
