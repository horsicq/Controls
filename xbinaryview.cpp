/* Copyright (c) 2019-2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "xbinaryview.h"

XBinaryView::XBinaryView(QWidget *pParent) : XHexView(pParent)
{
    m_fileType = XBinary::FT_UNKNOWN;
    m_bIsImage = false;
    m_nModuleAddress = -1;
}

XBinaryView::~XBinaryView()
{
}

void XBinaryView::setData(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    m_fileType = fileType;
    m_bIsImage = bIsImage;
    m_nModuleAddress = nModuleAddress;

    XHexView::OPTIONS hexOptions = {};
    hexOptions.nStartOffset = 0;
    hexOptions.nTotalSize = 0;
    hexOptions.nStartSelectionOffset = -1;
    hexOptions.nSizeOfSelection = 0;
    hexOptions.bMenu_Disasm = false;
    hexOptions.bMenu_MemoryMap = false;
    hexOptions.bMenu_MainHex = false;
    hexOptions.sTitle = "";
    hexOptions.addressMode = XHexView::LOCMODE_OFFSET;

    if (pDevice) {
        hexOptions.nTotalSize = pDevice->size();

        if (bIsImage) {
            hexOptions.addressMode = XHexView::LOCMODE_ADDRESS;
        }
    }

    XHexView::setData(pDevice, hexOptions, true, nullptr);
}
