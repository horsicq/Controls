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
#ifdef QT_SQL_LIB
#include "dialogbookmarks.h"
#endif

class XDeviceTableView : public XAbstractTableView {
    Q_OBJECT

public:
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
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
        VIEWWIDGET_SCRIPTS,
#endif
    };

    XDeviceTableView(QWidget *pParent = nullptr);

    void setXInfoDB(XInfoDB *pXInfoDB);
    XInfoDB *getXInfoDB();
    void setDevice(QIODevice *pDevice);
    void setBackupDevice(QIODevice *pDevice);
    QIODevice *getDevice();
    QIODevice *getBackupDevice();
    void setViewSize(qint64 nViewSize);
    qint64 getViewSize();
    void setMemoryMap(const XBinary::_MEMORY_MAP &memoryMap);
    XBinary::_MEMORY_MAP *getMemoryMap();
    void setAddressMode(LOCMODE addressMode);
    LOCMODE getAddressMode();
    qint64 write_array(qint64 nOffset, char *pData, qint64 nDataSize);
    QByteArray read_array(qint64 nOffset, qint32 nSize);
    void goToAddress(XADDR nAddress, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void goToOffset(qint64 nOffset, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void setSelectionAddress(XADDR nAddress, qint64 nSize);
    void setSelectionOffset(qint64 nOffset, qint64 nSize);
    bool isEdited();
    bool saveBackup();
    void setReadonly(bool bState);
    bool isReadonly();
    void adjustAfterAnalysis();  // TODO Check mb remove
    virtual DEVICESTATE getDeviceState(bool bGlobalOffset = false);
    virtual void setDeviceState(const DEVICESTATE &deviceState, bool bGlobalOffset = false);
    virtual qint64 deviceOffsetToViewOffset(qint64 nOffset, bool bGlobalOffset = false);
    virtual qint64 deviceSizeToViewSize(qint64 nOffset, qint64 nSize, bool bGlobalOffset = false);  // TODO mb remove
    virtual qint64 viewOffsetToDeviceOffset(qint64 nViewOffset, bool bGlobalOffset = false);
    void setDeviceSelection(qint64 nOffset, qint64 nSize);
    virtual qint64 deviceOffsetToGlobal(qint64 nDeviceOffset);
    bool isPrevVisitedAvailable();
    bool isNextVisitedAvailable();
    void goToNextVisited();
    void goToPrevVisited();
    void addVisited(qint64 nViewOffset);
    void clearVisited();

    static QList<HIGHLIGHTREGION> _convertBookmarksToHighlightRegion(QList<XInfoDB::BOOKMARKRECORD> *pList);
    static QList<HIGHLIGHTREGION> getHighlightRegion(QList<HIGHLIGHTREGION> *pList, quint64 nLocation, XBinary::LT locationType);

    void setViewWidgetState(VIEWWIDGET viewWidget, bool bState);
    bool getViewWidgetState(VIEWWIDGET viewWidget);

    void dumpMemory(const QString &sDumpName, qint64 nOffset = 0, qint64 nSize = -1);

public slots:
    void setEdited(qint64 nDeviceOffset, qint64 nDeviceSize);
    void _goToAddressSlot();
    void _goToOffsetSlot();

protected:
    virtual bool isViewOffsetValid(qint64 nViewOffset);
    virtual bool isEnd(qint64 nOffset);
    virtual void adjustScrollCount();
    virtual qint64 getViewSizeByViewOffset(qint64 nViewOffset);
    virtual qint64 addressToViewOffset(XADDR nAddress);

signals:
    void visitedStateChanged();
    void dataChanged(qint64 nDeviceOffset, qint64 nDeviceSize);
    void deviceSizeChanged(qint64 nOldSize, qint64 nNewSize);
    void deviceSelectionChanged(qint64 nDeviceOffset, qint64 nDeviceSize);
    void viewWidgetsStateChanged();
    void closeWidget_DataInspector();
    void closeWidget_DataConvertor();
    void closeWidget_Multisearch();
    void closeWidget_Strings();
#ifdef QT_SQL_LIB
    void closeWidget_Bookmarks();
#endif
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
#ifdef QT_SQL_LIB
    void _bookmarkNew();
    void _bookmarkList();
#endif
    void currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize);

public slots:
    void _showDataInspector();
    void _showDataConvertor();
    void _showMultisearch();

private:
    static const qint32 N_MAX_VISITED = 100;
    XInfoDB *g_pXInfoDB;
    QIODevice *g_pDevice;
    QIODevice *g_pBackupDevice;
    qint64 g_nViewSize;
    XBinary::_MEMORY_MAP g_memoryMap;
    XBinary::SEARCHDATA g_searchData;
    LOCMODE g_addressMode;
    bool g_bIsReadonly;
    QList<qint64> g_listVisited;
    qint32 g_nVisitedIndex;
    static QSet<VIEWWIDGET> g_stViewWidgetState;
};

#endif  // XDEVICETABLEVIEW_H
