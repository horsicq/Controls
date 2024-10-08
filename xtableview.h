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
#ifndef XTABLEVIEW_H
#define XTABLEVIEW_H

#include <QTableView>
#ifdef QT_CONCURRENT_LIB
#include <QAbstractItemModel>
#include <QFuture>
#include <QStandardItemModel>
#include <QtConcurrent>
#endif

#include "xheaderview.h"
#include "xsortfilterproxymodel.h"

class XTableView : public QTableView {
    Q_OBJECT

public:
    XTableView(QWidget *pParent = nullptr);
    ~XTableView();

    void setCustomModel(QStandardItemModel *pModel, bool bFilterEnabled);
    void deleteOldModel(QStandardItemModel **g_ppOldModel);
    XSortFilterProxyModel *getProxyModel();

private slots:
    void onFilterChanged();

private:
    bool g_bFilterEnabled;
    XHeaderView *g_pHeaderView;
    XSortFilterProxyModel *g_pSortFilterProxyModel;
    QStandardItemModel *g_pOldModel;
    QStandardItemModel *g_pModel;
};

#endif  // XTABLEVIEW_H
