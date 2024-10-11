/* Copyright (c) 2024 hors<horsicq@gmail.com>
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
    g_bFilterEnabled = false;
    g_pOldModel = nullptr;
    g_pModel = nullptr;
    g_pHeaderView = new XHeaderView(this);
    g_pSortFilterProxyModel = new XSortFilterProxyModel(this);

    setHorizontalHeader(g_pHeaderView);

    connect(g_pHeaderView, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));

    setSortingEnabled(true);
}

XTableView::~XTableView()
{
}

void XTableView::setCustomModel(QStandardItemModel *pModel, bool bFilterEnabled)
{
    // TODO Stretch last section
    g_bFilterEnabled = bFilterEnabled;

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
        deleteOldModel(&g_pOldModel);
    }

    g_pModel = pModel;

    if (bFilterEnabled) {
        g_pHeaderView->setNumberOfFilters(pModel->columnCount());
        g_pSortFilterProxyModel->setSourceModel(pModel);
        setModel(g_pSortFilterProxyModel);
    } else {
        setModel(pModel);
    }
}

void XTableView::deleteOldModel(QStandardItemModel **g_ppOldModel)
{
    delete (*g_ppOldModel);

    (*g_ppOldModel) = nullptr;
}

XSortFilterProxyModel *XTableView::getProxyModel()
{
    return g_pSortFilterProxyModel;
}

void XTableView::onFilterChanged()
{
    QList<QString> listFilters = g_pHeaderView->getFilters();

    g_pSortFilterProxyModel->setFilters(listFilters);
}
