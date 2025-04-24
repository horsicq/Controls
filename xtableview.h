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
#ifndef XTABLEVIEW_H
#define XTABLEVIEW_H

#include <QTableView>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QElapsedTimer>

#include "xheaderview.h"
#include "xsortfilterproxymodel.h"
#include "xmodel.h"
#include "xoptions.h"

class XTableView : public QTableView {
    Q_OBJECT

public:
    XTableView(QWidget *pParent = nullptr);
    ~XTableView();

    void setCustomModel(QAbstractItemModel *pModel, bool bFilterEnabled);
    void clear();
    XSortFilterProxyModel *getProxyModel();
    void setFilterEnabled(qint32 nColumn, bool bFilterEnabled);
    void adjust();

private:
    void deleteOldModel(QAbstractItemModel **g_ppOldModel);

private slots:
    void onFilterChanged();
    void onSortChanged(int column, Qt::SortOrder order);
    void horisontalScroll();

private:
    XHeaderView *g_pHeaderView;
    XSortFilterProxyModel *g_pSortFilterProxyModel;
    QAbstractItemModel *g_pOldModel;
    QAbstractItemModel *g_pModel;
};

#endif  // XTABLEVIEW_H
