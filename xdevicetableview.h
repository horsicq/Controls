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
#ifndef XDEVICETABLEVIEW_H
#define XDEVICETABLEVIEW_H

#include "dialogdatainspector.h"
#include "dialogxdataconvertor.h"
#include "dialogdumpprocess.h"
#include "dialoggotoaddress.h"
#include "dialoghexsignature.h"
#include "dialogsearch.h"
#include "dialogshowdata.h"
#include "dialogsearchprocess.h"
#include "xabstracttableview.h"
#include "dialogresize.h"
#include "dialogremove.h"
#include "xformats.h"
#include "xinfodb.h"
#include "dialogsearchvalues.h"
#include "dialogvisualization.h"
#include "dialogbookmarks.h"

class XDeviceTableView : public XAbstractTableView {
    Q_OBJECT

public:
    struct VIEWSTRUCT {
        qint64 nViewPos;
        XADDR nAddress;
        qint64 nOffset;
        qint64 nSize;
    };

    enum LOCMODE {
        LOCMODE_OFFSET = 0,
        LOCMODE_ADDRESS,
        LOCMODE_RELADDRESS,
        LOCMODE_THIS
    };

    struct DEVICESTATE {
        quint64 nSelectionDeviceOffset;
        qint64 nSelectionSize;
        //        qint64 nCursorOffset;
        qint64 nStartDeviceOffset;
    };

    struct HIGHLIGHTREGION {
        bool bIsValid;
        quint64 nLocation;
        XBinary::LT locationType;
        qint64 nSize;
        QColor colText;
        QColor colBackground;
        QColor colBackgroundSelected;
        QString sComment;
    };

    enum VIEWWIDGET {
        VIEWWIDGET_DATAINSPECTOR,
        VIEWWIDGET_DATACONVERTOR,
        VIEWWIDGET_MULTISEARCH,
        VIEWWIDGET_BOOKMARKS,
        VIEWWIDGET_STRINGS,
        VIEWWIDGET_VISUALIZATION,
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
        VIEWWIDGET_SCRIPTS,
#endif
    };

    XDeviceTableView(QWidget *pParent = nullptr);
    ~XDeviceTableView();

    void setXInfoDB(XInfoDB *pXInfoDB);
    XInfoDB *getXInfoDB();
    void setMode(XBinary::FT fileType, XBinary::DM disasmMode);
    void setDevice(QIODevice *pDevice, qint64 nStartOffset, qint64 nTotalSize);
    QIODevice *getDevice();
    void setViewSize(qint64 nViewSize);
    qint64 getViewSize();
    XBinary::_MEMORY_MAP *getMemoryMap();
    void setLocationMode(LOCMODE locationMode);
    LOCMODE getlocationMode();
    void setLocationBase(qint32 nBase);
    qint32 getLocationBase();
    qint64 write_array(qint64 nOffset, char *pData, qint64 nDataSize);
    QByteArray read_array(qint64 nOffset, qint32 nSize);
    void goToAddress(XADDR nAddress, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void goToOffset(qint64 nOffset, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void goToLocation(XADDR nLocation, XBinary::LT locationType, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void setLocationOffset(XADDR nLocation, XBinary::LT locationType, qint64 nSize);
    void setSelectionAddress(XADDR nAddress, qint64 nSize);        // TODO remove
    void setSelectionRelAddress(XADDR nRelAddress, qint64 nSize);  // TODO remove
    void setSelectionOffset(qint64 nOffset, qint64 nSize);         // TODO remove
    bool isEdited();
    bool saveBackup();
    void adjustAfterAnalysis();  // TODO Check mb remove
    virtual DEVICESTATE getDeviceState(bool bGlobalOffset = false);
    virtual void setDeviceState(const DEVICESTATE &deviceState, bool bGlobalOffset);
    qint64 deviceOffsetToViewPos(qint64 nOffset);
    qint64 deviceSizeToViewSize(qint64 nOffset, qint64 nSize);  // TODO mb remove
    qint64 viewPosToDeviceOffset(qint64 nViewPos);
    void setDeviceSelection(qint64 nOffset, qint64 nSize);
    virtual qint64 deviceOffsetToGlobal(qint64 nDeviceOffset);
    bool isPrevVisitedAvailable();
    bool isNextVisitedAvailable();
    void goToNextVisited();
    void goToPrevVisited();
    void addVisited(qint64 nViewPos);
    void clearVisited();

    static QList<HIGHLIGHTREGION> _convertBookmarksToHighlightRegion(QList<XInfoDB::BOOKMARKRECORD> *pList);
    static QList<HIGHLIGHTREGION> getHighlightRegion(QList<HIGHLIGHTREGION> *pList, quint64 nLocation, XBinary::LT locationType);

    void setViewWidgetState(VIEWWIDGET viewWidget, bool bState);
    bool getViewWidgetState(VIEWWIDGET viewWidget);

    void dumpMemory(const QString &sDumpName, qint64 nOffset = 0, qint64 nSize = -1);
    virtual void setLocation(quint64 nLocation, qint32 nLocationType, qint64 nSize);

    VIEWSTRUCT _getViewStructByOffset(qint64 nOffset);
    VIEWSTRUCT _getViewStructByAddress(XADDR nAddress);
    // VIEWSTRUCT _getViewStructByScroll(qint64 nValue);
    VIEWSTRUCT _getViewStructByViewPos(qint64 nViewPos);

public slots:
    void setEdited(qint64 nDeviceOffset, qint64 nDeviceSize);
    void _goToAddressSlot();
    void _goToOffsetSlot();

protected:
    virtual bool isViewPosValid(qint64 nViewPos);
    virtual bool isEnd(qint64 nOffset);
    virtual void adjustScrollCount();
    virtual qint64 getViewSizeByViewPos(qint64 nViewPos);
    virtual qint64 addressToViewPos(XADDR nAddress);
    virtual qint64 locationToViewPos(XADDR nLocation, XBinary::LT locationType);

signals:
    void visitedStateChanged();
    void deviceSizeChanged(qint64 nOldSize, qint64 nNewSize);
    void viewWidgetsStateChanged();
    void closeWidget_DataInspector();
    void closeWidget_DataConvertor();
    void closeWidget_Multisearch();
    void closeWidget_Strings();
    void closeWidget_Visualization();
    void closeWidget_Bookmarks();
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
    void closeWidget_Scripts();
#endif

protected slots:
    void _goToSelectionStart();
    void _goToSelectionEnd();
    void _dumpToFileSlot();
    void _hexSignatureSlot();
    void _findStringSlot();
    void _findSignatureSlot();
    void _findValueSlot();
    void _findSlot(DialogSearch::SEARCHMODE mode);
    void _findNextSlot();
    void _selectAllSlot();
    void _copyDataSlot();
    void _copyAddressSlot();
    void _copyRelAddressSlot();
    void _copyOffsetSlot();
    void _setEdited(qint64 nDeviceOffset, qint64 nDeviceSize);
    void goToAddressSlot(XADDR nAddress, qint64 nSize);
    void goToAddressSlot(XADDR nAddress);
    void reloadView();
    void selectionChangedSlot();
    void changeLocationMode();
    void changeLocationBase();
    void _bookmarkNew();
    void _bookmarkList();

public slots:
    void currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize);
    void _showDataInspector();
    void _showDataConvertor();
    void _showMultisearch();

private:
    static const qint32 N_MAX_VISITED = 100;
    XInfoDB g_emptyXInfoDB;
    XInfoDB *g_pXInfoDB;
    QIODevice *g_pDevice;
    qint64 g_nStartOffset;
    qint64 g_nTotalSize;
    qint64 g_nViewSize;
    XBinary::SEARCHDATA g_searchData;
    LOCMODE g_locationMode;
    qint32 g_nLocationBase;
    QList<qint64> g_listVisited;
    qint32 g_nVisitedIndex;
    QSet<VIEWWIDGET> g_stViewWidgetState;
    XBinary::FT g_fileType;
    XBinary::DM g_disasmMode;
    XBinary::_MEMORY_MAP g_memoryMap;
    QList<VIEWSTRUCT> g_listViewStruct;
};

#endif  // XDEVICETABLEVIEW_H
