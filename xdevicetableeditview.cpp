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

        SubDevice sd(getDevice(), state.nSelectionOffset, state.nSelectionSize);

        if (sd.open(QIODevice::ReadWrite)) {
            DialogHexEdit dialogHexEdit(this);

            dialogHexEdit.setGlobal(getShortcuts(), getGlobalOptions());

            //        connect(&dialogHexEdit,SIGNAL(changed()),this,SLOT(_setEdited()));

            dialogHexEdit.setData(&sd, state.nSelectionOffset);
            dialogHexEdit.setBackupDevice(getBackupDevice());

            dialogHexEdit.exec();

            _setEdited(state.nSelectionOffset, state.nSelectionSize);

            sd.close();
        }
    }
}

void XDeviceTableEditView::_followInDisasmSlot()
{
    qint64 nOffset = getDeviceState(true).nCursorOffset;
    //    XADDR nAddress=XBinary::offsetToAddress(getMemoryMap(),nOffset);

    emit followInDisasm(nOffset);
}

void XDeviceTableEditView::_followInHexSlot()
{
    //    emit
    //    followInHex(XBinary::offsetToAddress(getMemoryMap(),getStateOffset()));
    qint64 nOffset = getDeviceState(true).nCursorOffset;
    //    XADDR nAddress=XBinary::offsetToAddress(getMemoryMap(),nOffset);

    emit followInHex(nOffset);
}
