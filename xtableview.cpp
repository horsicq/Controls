/* Copyright (c) 2024-2025 hors<horsicq@gmail.com>
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
#include "xtableview.h"

XTableView::XTableView(QWidget *pParent) : QTableView(pParent)
{
    g_pOldModel = nullptr;
    g_pModel = nullptr;
    g_pHeaderView = new XHeaderView(this);
    g_pSortFilterProxyModel = new XSortFilterProxyModel(this);
    g_bIsXmodel = false;

    setHorizontalHeader(g_pHeaderView);

    connect(g_pHeaderView, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));
    connect(g_pHeaderView, SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(onSortChanged(int, Qt::SortOrder)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horisontalScroll()));

    setSortingEnabled(true);
    setWordWrap(false);
}

XTableView::~XTableView()
{
}

void XTableView::setCustomModel(QAbstractItemModel *pModel, bool bFilterEnabled)
{
    // TODO Stretch last section
    g_pOldModel = g_pModel;

    if (g_pOldModel) {
        // #ifdef QT_CONCURRENT_LIB
        // #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        //         QtConcurrent::run(&XTableView::deleteOldModel, this, &g_pOldModel);
        // #else
        //         QtConcurrent::run(this, &XTableView::deleteOldModel, &g_pOldModel);
        // #endif
        // #else
        //         deleteOldModel(&g_pOldModel);
        // #endif
        // g_pOldModel->beginResetModel();
#ifdef QT_DEBUG
        // get elapsed time
        QElapsedTimer timer;
        timer.start();
        qDebug("g_pOldModel->clear() START");
        // QStandartItemModel 433543
        // 470701 ms
#endif
        // g_pOldModel->clear();
        deleteOldModel(&g_pOldModel);
#ifdef QT_DEBUG
        qDebug("setCustomModel Elapsed time: %lld ms", timer.elapsed());
#endif
        // g_pOldModel->endResetModel();
    }

    g_pModel = pModel;

    if (bFilterEnabled) {
        g_pHeaderView->setNumberOfFilters(pModel->columnCount());
        g_pSortFilterProxyModel->setSourceModel(pModel);
        setModel(g_pSortFilterProxyModel);
    } else {
        setModel(pModel);
    }

    adjust();
}

void XTableView::clear()
{
    setModel(nullptr);
}

void XTableView::deleteOldModel(QAbstractItemModel **g_ppOldModel)
{
    delete (*g_ppOldModel);

    (*g_ppOldModel) = nullptr;
}

XSortFilterProxyModel *XTableView::getProxyModel()
{
    return g_pSortFilterProxyModel;
}

void XTableView::setFilterEnabled(qint32 nColumn, bool bFilterEnabled)
{
    g_pHeaderView->setFilterEnabled(nColumn, bFilterEnabled);
}

void XTableView::adjust()
{
    XModel *pModel = dynamic_cast<XModel *>(g_pModel);

    if (pModel) {
        qint32 nNumberOfColumns = pModel->columnCount();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            qint32 nSymbolSize = pModel->getColumnSymbolSize(i);

            if (nSymbolSize != -1) {
                qint32 nWidth = XOptions::getControlWidth(this, nSymbolSize);
                setColumnWidth(i, nWidth);
            }
        }
        g_bIsXmodel = true;
    } else {
        g_bIsXmodel = false;
    }
}

void XTableView::onFilterChanged()
{
#ifdef QT_DEBUG
    // Elapsed time
    QElapsedTimer timer;
    timer.start();
    qDebug("onFilterChanged START");
#endif

    QList<QString> listFilters = g_pHeaderView->getFilters();

    g_pSortFilterProxyModel->setFilters(listFilters);

    if (g_bIsXmodel) {
        // TODO Thread
        g_pSortFilterProxyModel->invalidate();
    } else {
        g_pSortFilterProxyModel->invalidate();
    }

#ifdef QT_DEBUG
    qDebug("onFilterChanged Elapsed time: %lld ms", timer.elapsed());
    // 16266 ms
    // 16342
    //
    //
#endif
}

void XTableView::onSortChanged(int column, Qt::SortOrder order)
{
    if (g_bIsXmodel) {
        // TODO Thread
        g_pSortFilterProxyModel->sort(column, order);
    } else {
        g_pSortFilterProxyModel->sort(column, order);
    }
}

void XTableView::horisontalScroll()
{
    g_pHeaderView->adjustPositions();
}
