/* Copyright (c) 2020-2024 hors<horsicq@gmail.com>
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

            dialogHexEdit.exec();

            _setEdited(state.nSelectionDeviceOffset, state.nSelectionSize);

            sd.close();
        }
    }
}

void XDeviceTableEditView::_editPatch()
{
    if (!isReadonly()) {
        QString sJsonFileName =
            QFileDialog::getOpenFileName(this, tr("Open file") + QString("..."), XBinary::getDeviceDirectory(getDevice()), QString("%1 (*.patch.json)").arg(tr("Patch")));

        if (sJsonFileName != "") {
            DialogDumpProcess dd(this);
            dd.setGlobal(getShortcuts(), getGlobalOptions());
            dd.setData(getDevice(), DumpProcess::DT_PATCH_DEVICE_OFFSET, sJsonFileName);

            dd.showDialogDelay();

            reload(true);
        }
    }
}

void XDeviceTableEditView::_editRemove()
{
    if (!isReadonly()) {
        if (XBinary::isResizeEnable(getDevice())) {
            DEVICESTATE state = getDeviceState();

            DialogRemove::DATA _data = {};
            _data.nOffset = state.nSelectionDeviceOffset;
            _data.nSize = state.nSelectionSize;
            _data.nMaxSize = getDevice()->size();

            DialogRemove dialogRemove(this, &_data);
            dialogRemove.setGlobal(getShortcuts(), getGlobalOptions());

            if (dialogRemove.exec() == QDialog::Accepted) {
                qint64 nOldSize = _data.nMaxSize;
                qint64 nNewSize = nOldSize - _data.nSize;

                if (nOldSize != nNewSize) {
                    if (saveBackup()) {
                        // mb TODO Process move memory
                        if (XBinary::moveMemory(getDevice(), _data.nOffset + _data.nSize, _data.nOffset, _data.nSize)) {
                            if (XBinary::resize(getDevice(), nNewSize)) {
                                // mb TODO correct bookmarks
                                adjustScrollCount();
                                reload(true);
                                emit deviceSizeChanged(nOldSize, nNewSize);
                                emit dataChanged(_data.nOffset, nNewSize - _data.nOffset); // TODO initOffset
                            }
                        }
                    }
                }
            }
        }
    }
}

void XDeviceTableEditView::_editResize()
{
    if (!isReadonly()) {
        if (XBinary::isResizeEnable(getDevice())) {
            DialogResize::DATA _data = {};
            _data.nOldSize = getDevice()->size();
            _data.nNewSize = _data.nOldSize;

            DialogResize dialogResize(this, &_data);
            dialogResize.setGlobal(getShortcuts(), getGlobalOptions());

            if (dialogResize.exec() == QDialog::Accepted) {
                if (_data.nOldSize != _data.nNewSize) {
                    if (saveBackup()) {
                        if (XBinary::resize(getDevice(), _data.nNewSize)) {
                            // mb TODO correct bookmarks
                            adjustScrollCount();
                            reload(true);
                            if (_data.nNewSize > _data.nOldSize) {
                                emit deviceSizeChanged(_data.nOldSize, _data.nNewSize);
                                emit dataChanged(_data.nOldSize, _data.nNewSize - _data.nOldSize); // TODO initOffset
                            } else if (_data.nOldSize > _data.nNewSize) {
                                emit deviceSizeChanged(_data.nOldSize, _data.nNewSize);
                                emit dataChanged(_data.nNewSize, _data.nOldSize - _data.nNewSize); // TODO initOffset
                            }
                        } else {
                            emit errorMessage(tr("Cannot resize"));
                        }
                    }
                }
            }
        }
    }
}

void XDeviceTableEditView::_strings()
{
    if (!getViewWidgetState(VIEWWIDGET_STRINGS)) {
        setViewWidgetState(VIEWWIDGET_STRINGS, true);

        DialogSearchStrings dialogSearchStrings(this);

        connect(&dialogSearchStrings, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));

        connect(this, SIGNAL(closeWidget_Strings()), &dialogSearchStrings, SLOT(close()));

        dialogSearchStrings.setGlobal(getShortcuts(), getGlobalOptions());

        SearchStringsWidget::OPTIONS stringsOptions = {};
        stringsOptions.bAnsi = true;
        // stringsOptions.bUTF8 = false;
        stringsOptions.bUnicode = true;
        stringsOptions.bNullTerminated = false;

        dialogSearchStrings.setData(getDevice(), XBinary::FT_UNKNOWN, stringsOptions, true);

        XOptions::_adjustStayOnTop(&dialogSearchStrings, true);

        dialogSearchStrings.exec();

        setViewWidgetState(VIEWWIDGET_STRINGS, false);
    } else {
        emit closeWidget_Strings();
    }
}

void XDeviceTableEditView::_visualization()
{
    if (!getViewWidgetState(VIEWWIDGET_VISUALIZATION)) {
        setViewWidgetState(VIEWWIDGET_VISUALIZATION, true);

        DialogVisualization dialogVisualization(this);

        connect(&dialogVisualization, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));

        connect(this, SIGNAL(closeWidget_Visualization()), &dialogVisualization, SLOT(close()));

        dialogVisualization.setGlobal(getShortcuts(), getGlobalOptions());

        dialogVisualization.setData(getDevice(), XBinary::FT_UNKNOWN, true);  // TODO options

        XOptions::_adjustStayOnTop(&dialogVisualization, true);

        dialogVisualization.exec();

        setViewWidgetState(VIEWWIDGET_VISUALIZATION, false);
    } else {
        emit closeWidget_Visualization();
    }
}
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
void XDeviceTableEditView::_scripts()
{
    if (!getViewWidgetState(VIEWWIDGET_SCRIPTS)) {
        setViewWidgetState(VIEWWIDGET_SCRIPTS, true);

        DialogDieHexViewer dialogDieHexViewer(this);

        connect(&dialogDieHexViewer, SIGNAL(currentAddressChanged(XADDR, qint64)), this, SLOT(goToAddressSlot(XADDR, qint64)));
        connect(this, SIGNAL(closeWidget_Scripts()), &dialogDieHexViewer, SLOT(close()));

        dialogDieHexViewer.setGlobal(getShortcuts(), getGlobalOptions());

        XOptions::_adjustStayOnTop(&dialogDieHexViewer, true);

        dialogDieHexViewer.exec();

        setViewWidgetState(VIEWWIDGET_SCRIPTS, false);
    } else {
        emit closeWidget_Scripts();
    }
}
#endif
