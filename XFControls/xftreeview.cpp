/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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

#include "xftreeview.h"

#include <QScrollBar>
#include <QSignalBlocker>

XFTreeView::XFTreeView(QWidget *pParent) : QTreeView(pParent)
{
    m_pTreeModel = nullptr;

    // Same filter-under-the-title mechanism as XTableView: the header itself
    // hosts one QLineEdit per column below the section labels.
    m_pHeaderView = new XHeaderView(this);
    setHeader(m_pHeaderView);
    // The tree model is not sortable - no clickable sections, no sort arrow
    m_pHeaderView->setSectionsClickable(false);
    m_pHeaderView->setSortIndicatorShown(false);
    // Restore the QTreeView default-header traits the swap discarded
    // (a plain QHeaderView is non-stretching and center-aligned)
    m_pHeaderView->setStretchLastSection(true);
    m_pHeaderView->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    connect(m_pHeaderView, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onHorizontalScroll()));

    // Debounce: crafted files can yield 100k+ tree nodes and the filter walk is
    // a synchronous full-tree pass - one pass per typing pause, not per keystroke
    m_timerFilter.setSingleShot(true);
    m_timerFilter.setInterval(250);
    connect(&m_timerFilter, SIGNAL(timeout()), this, SLOT(onFilterTimerTimeout()));

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);
    setUniformRowHeights(true);
    setWordWrap(false);
    setAccessibleName(tr("File structures and analysis tools"));
    setAccessibleDescription(tr("Select a structure or analysis tool to show its details"));
}

XFTreeView::~XFTreeView()
{
    clear();
}

void XFTreeView::setData(const XBinary::INDATA &inData, const QList<XBinary::XFHEADER> &listHeaders, bool bExtraInfo)
{
    clear();

    m_inData = inData;

    m_pTreeModel = new XFTreeModel(this);
    m_pTreeModel->setData(m_inData, listHeaders, bExtraInfo);
    setModel(m_pTreeModel);

    // Keep one filter edit per column; recreating wipes the texts, so only do it
    // when the column count actually changed, and carry the texts over silently.
    qint32 nNumberOfColumns = m_pTreeModel->columnCount();
    QList<QString> listOldFilters = m_pHeaderView->getFilters();

    if (listOldFilters.count() != nNumberOfColumns) {
        QSignalBlocker blocker(m_pHeaderView);
        m_pHeaderView->setNumberOfFilters(nNumberOfColumns);

        qint32 nRestoreCount = qMin(listOldFilters.count(), nNumberOfColumns);
        for (qint32 i = 0; i < nRestoreCount; i++) {
            m_pHeaderView->setFilterText(i, listOldFilters.at(i));
        }
    }

    // Expand first so the column-width measurement sees the whole tree
    expandAll();

    adjust();

    // Fresh model starts unfiltered - re-apply what is typed in the header
    _refreshFilters();
}

void XFTreeView::clear()
{
    setModel(nullptr);

    if (m_pTreeModel) {
        delete m_pTreeModel;
        m_pTreeModel = nullptr;
    }
}

void XFTreeView::adjust()
{
    qint32 nColumnCount = header()->count();

    for (qint32 i = 0; i < nColumnCount; i++) {
        resizeColumnToContents(i);
    }
}

void XFTreeView::selectFirstItem()
{
    if (m_pTreeModel) {
        qint32 nRowCount = m_pTreeModel->rowCount();

        // Skip rows hidden by an active filter; select nothing if all are hidden
        for (qint32 i = 0; i < nRowCount; i++) {
            if (!isRowHidden(i, QModelIndex())) {
                setCurrentIndex(m_pTreeModel->index(i, 0));
                break;
            }
        }
    }
}

void XFTreeView::clearFilters()
{
    bool bWasActive = _hasActiveFilter();

    {
        QSignalBlocker blocker(m_pHeaderView);
        m_pHeaderView->clearFilters();
    }

    m_listFilters.clear();

    if (m_pTreeModel && bWasActive) {
        _applyFilter(QModelIndex());
    }
}

XFTreeModel *XFTreeView::getTreeModel()
{
    return m_pTreeModel;
}

XBinary::XFHEADER XFTreeView::getSelectedHeader()
{
    XBinary::XFHEADER result = {};

    QModelIndex currentIndex = this->currentIndex();

    if (currentIndex.isValid() && m_pTreeModel) {
        const XFTreeModel::TREEITEM *pItem = m_pTreeModel->itemFromIndex(currentIndex);

        if (pItem) {
            result = pItem->xfHeader;
        }
    }

    return result;
}

void XFTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QTreeView::currentChanged(current, previous);

    if (current.isValid() && m_pTreeModel) {
        const XFTreeModel::TREEITEM *pItem = m_pTreeModel->itemFromIndex(current);

        if (pItem) {
            emit headerSelected(pItem->xfHeader);
        }
    }
}

void XFTreeView::onFilterChanged()
{
    m_timerFilter.start();
}

void XFTreeView::onFilterTimerTimeout()
{
    _refreshFilters();
}

void XFTreeView::onHorizontalScroll()
{
    m_pHeaderView->adjustPositions();
}

bool XFTreeView::_hasActiveFilter() const
{
    bool bResult = false;

    qint32 nCount = m_listFilters.count();

    for (qint32 i = 0; i < nCount; i++) {
        if (!m_listFilters.at(i).isEmpty()) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

void XFTreeView::_refreshFilters()
{
    bool bWasActive = _hasActiveFilter();

    m_listFilters = m_pHeaderView->getFilters();

    bool bIsActive = _hasActiveFilter();

    if (m_pTreeModel == nullptr) {
        return;
    }

    // Nothing was hidden and nothing will be - skip the full-tree walk.
    // Do NOT add a same-content early-out on top of this: setData()
    // deliberately re-applies the unchanged filters to a fresh model.
    if (!bWasActive && !bIsActive) {
        return;
    }

    _applyFilter(QModelIndex());
}

bool XFTreeView::_applyFilter(const QModelIndex &parentIndex)
{
    // A row stays visible if every non-empty column filter matches its column
    // (same AND semantics as the table view) or any descendant is visible;
    // no active filter makes every row visible. View-level hiding only - the
    // model and the current selection are left untouched.
    bool bAnyVisible = false;
    bool bIsActive = _hasActiveFilter();
    qint32 nRowCount = m_pTreeModel->rowCount(parentIndex);
    qint32 nFilterCount = m_listFilters.count();

    for (qint32 i = 0; i < nRowCount; i++) {
        QModelIndex index = m_pTreeModel->index(i, 0, parentIndex);

        bool bChildVisible = _applyFilter(index);
        bool bSelfMatch = true;

        if (bIsActive) {
            for (qint32 j = 0; j < nFilterCount; j++) {
                const QString &sFilter = m_listFilters.at(j);

                if (sFilter.isEmpty()) {
                    continue;
                }

                QString sText = m_pTreeModel->data(m_pTreeModel->index(i, j, parentIndex), Qt::DisplayRole).toString();

                if (!sText.contains(sFilter, Qt::CaseInsensitive)) {
                    bSelfMatch = false;
                    break;
                }
            }
        }

        bool bVisible = (bSelfMatch || bChildVisible);

        setRowHidden(i, parentIndex, !bVisible);

        // Matches inside a user-collapsed branch must become visible
        if (bChildVisible && bIsActive) {
            expand(index);
        }

        if (bVisible) {
            bAnyVisible = true;
        }
    }

    return bAnyVisible;
}
