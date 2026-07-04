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

#include "xftableview.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

XFTableView::XFTableView(QWidget *pParent) : QWidget(pParent)
{
    m_pHeaderModel = nullptr;
    m_pTableModel = nullptr;
    m_bSortActive = false;

    m_pTableView = new XTableView(this);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pTableView->setThreadedFilterSortEnabled(true);

    m_pToolBar = new QToolBar(this);
    m_pToolBar->setMovable(false);
    m_pToolBar->setFloatable(false);
    m_pToolBar->setIconSize(QSize(16, 16));

    m_pProgressBar = new QProgressBar(m_pToolBar);
    m_pProgressBar->setRange(0, 100);
    m_pProgressBar->setValue(0);
    m_pProgressBar->setMaximumWidth(150);
    m_pProgressBar->setVisible(false);
    m_pToolBar->addWidget(m_pProgressBar);

    QWidget *pSpacer = new QWidget(m_pToolBar);
    pSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_pToolBar->addWidget(pSpacer);

    m_pActionResetFilter = m_pToolBar->addAction(tr("Reset filter"));
    m_pActionResetFilter->setEnabled(false);

    m_pActionSave = m_pToolBar->addAction(tr("Save"));

    m_pStatusBar = new QStatusBar(this);
    m_pStatusBar->setSizeGripEnabled(false);
    m_pStatusBar->setVisible(false);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSpacing(0);
    pLayout->addWidget(m_pToolBar);
    pLayout->addWidget(m_pTableView);
    pLayout->addWidget(m_pStatusBar);

    connect(m_pTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    connect(m_pTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
    connect(m_pTableView, SIGNAL(busyChanged(bool)), this, SLOT(onTableBusyChanged(bool)));
    connect(m_pActionSave, SIGNAL(triggered()), this, SLOT(onSaveClicked()));
    connect(m_pActionResetFilter, SIGNAL(triggered()), this, SLOT(onResetFilterClicked()));

    XHeaderView *pHeaderView = qobject_cast<XHeaderView *>(m_pTableView->horizontalHeader());

    if (pHeaderView) {
        connect(pHeaderView, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));
        connect(pHeaderView, SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(onSortIndicatorChanged(int, Qt::SortOrder)));
    }
}

XFTableView::~XFTableView()
{
    clear();
}

void XFTableView::setData(const XFormats::INDATA &inData, const XBinary::XFHEADER &xfHeader)
{
    // setCustomModel() deletes the previous model itself
    m_pHeaderModel = nullptr;
    m_pTableModel = nullptr;

    m_inData = inData;
    m_xfHeader = xfHeader;

    if (m_xfHeader.xfType == XBinary::XFTYPE_HEADER) {
        m_pHeaderModel = new XFModel_header(this);
        m_pHeaderModel->setData(m_inData, m_xfHeader);
        m_pTableView->setCustomModel(m_pHeaderModel, true);
    } else if (m_xfHeader.xfType == XBinary::XFTYPE_TABLE) {
        m_pTableModel = new XFModel_table(this);
        m_pTableModel->setShowPresentation(true);
        m_pTableModel->setData(m_inData, m_xfHeader);
        m_pTableView->setCustomModel(m_pTableModel, true);
    }

    connect(m_pTableView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex, QModelIndex)), Qt::UniqueConnection);

    m_bSortActive = false;
    updateResetFilterEnabled();
}

void XFTableView::clear()
{
    m_pHeaderModel = nullptr;
    m_pTableModel = nullptr;
    m_pTableView->clear();
    setStatusBarText(QString());
}

void XFTableView::adjust()
{
    m_pTableView->adjust();
}

QAbstractItemModel *XFTableView::model() const
{
    return m_pTableView->model();
}

void XFTableView::setShowOffset(bool bShowOffset)
{
    if (m_pTableModel) {
        m_pTableModel->setShowOffset(bShowOffset);
        adjust();
    }
}

void XFTableView::setShowPresentation(bool bShowPresentation)
{
    if (m_pTableModel) {
        m_pTableModel->setShowPresentation(bShowPresentation);
        adjust();
    }
}

void XFTableView::setStretchLastColumn(bool bState)
{
    m_pTableView->horizontalHeader()->setStretchLastSection(bState);
}

void XFTableView::resizeColumnToContents(int column)
{
    QHeaderView *pHeader = m_pTableView->horizontalHeader();

    if (pHeader->stretchLastSection() && (column == pHeader->count() - 1)) {
        return;
    }

    m_pTableView->resizeColumnToContents(column);

    qint32 nHeaderWidth = pHeader->sectionSizeHint(column);

    if (nHeaderWidth > pHeader->sectionSize(column)) {
        pHeader->resizeSection(column, nHeaderWidth);
    }
}

void XFTableView::resizeColumnsToContents()
{
    qint32 nColumnCount = m_pTableView->horizontalHeader()->count();

    for (qint32 i = 0; i < nColumnCount; i++) {
        resizeColumnToContents(i);
    }
}

void XFTableView::onCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)

    if (current.isValid()) {
        qint32 nRow = m_pTableView->getProxyModel()->mapToSource(current).row();

        if (m_xfHeader.xfType == XBinary::XFTYPE_HEADER) {
            if (m_pHeaderModel && (nRow >= 0) && (nRow < m_xfHeader.listFields.count())) {
                XBinary::XFRECORD xfRecord = m_xfHeader.listFields.at(nRow);
                QVariant value = m_pHeaderModel->getFieldValue(nRow);
                emit fieldSelected(nRow, value, xfRecord);
            }
        } else if (m_xfHeader.xfType == XBinary::XFTYPE_TABLE) {
            emit rowSelected(nRow);
        }
    }
}

void XFTableView::onDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    qint32 nRow = m_pTableView->getProxyModel()->mapToSource(index).row();

    if (m_xfHeader.xfType == XBinary::XFTYPE_HEADER) {
        if (m_pHeaderModel && (nRow >= 0) && (nRow < m_xfHeader.listFields.count())) {
            XBinary::XFRECORD xfRecord = m_xfHeader.listFields.at(nRow);
            QVariant value = m_pHeaderModel->getFieldValue(nRow);
            emit fieldDoubleClicked(nRow, value, xfRecord);
        }
    }
}

void XFTableView::onCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    // TODO: context menu for copy, edit, etc.
}

void XFTableView::onFilterChanged()
{
    updateResetFilterEnabled();
}

void XFTableView::onSortIndicatorChanged(int nColumn, Qt::SortOrder order)
{
    Q_UNUSED(order)

    m_bSortActive = (nColumn >= 0);
    updateResetFilterEnabled();
}

void XFTableView::onResetFilterClicked()
{
    XHeaderView *pHeaderView = qobject_cast<XHeaderView *>(m_pTableView->horizontalHeader());

    if (pHeaderView) {
        pHeaderView->clearFilters();
    }

    m_pTableView->sortByColumn(-1, Qt::AscendingOrder);

    m_bSortActive = false;
    updateResetFilterEnabled();
}

void XFTableView::updateResetFilterEnabled()
{
    bool bFilterActive = false;

    XHeaderView *pHeaderView = qobject_cast<XHeaderView *>(m_pTableView->horizontalHeader());

    if (pHeaderView) {
        QList<QString> listFilters = pHeaderView->getFilters();

        for (const QString &sFilter : listFilters) {
            if (!sFilter.isEmpty()) {
                bFilterActive = true;
                break;
            }
        }
    }

    m_pActionResetFilter->setEnabled(m_bSortActive || bFilterActive);
}

void XFTableView::onTableBusyChanged(bool bBusy)
{
    if (bBusy) {
        m_pProgressBar->setRange(0, 0);  // indeterminate/"marquee" mode, no exact progress available
        m_pProgressBar->setVisible(true);
    } else {
        m_pProgressBar->setRange(0, 100);
        m_pProgressBar->setValue(0);
        m_pProgressBar->setVisible(false);
    }
}

void XFTableView::setProgressVisible(bool bVisible)
{
    m_pProgressBar->setVisible(bVisible);
}

void XFTableView::setProgressRange(qint32 nMinimum, qint32 nMaximum)
{
    m_pProgressBar->setRange(nMinimum, nMaximum);
}

void XFTableView::setProgressValue(qint32 nValue)
{
    m_pProgressBar->setValue(nValue);
}

void XFTableView::setStatusBarText(const QString &sText)
{
    if (sText.isEmpty()) {
        m_pStatusBar->clearMessage();
        m_pStatusBar->setVisible(false);
    } else {
        m_pStatusBar->showMessage(sText);
        m_pStatusBar->setVisible(true);
    }
}

void XFTableView::onSaveClicked()
{
    QAbstractItemModel *pModel = m_pTableView->model();

    if (!pModel) {
        return;
    }

    QString sDefaultName = m_pStatusBar->currentMessage();

    if (sDefaultName.isEmpty()) {
        sDefaultName = tr("Result");
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    sDefaultName.replace(QRegularExpression(QStringLiteral(R"([\\/:*?"<>|])")), QStringLiteral("_"));
#else
    sDefaultName.replace(QRegExp(QStringLiteral("[\\\\/:*?\"<>|]")), QStringLiteral("_"));
#endif

    QString sSelectedFilter;
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save"), sDefaultName, XFModel::exportAllFilters(), &sSelectedFilter);

    if (sFileName.isEmpty()) {
        return;
    }

    XFModel::EXPORT_FORMAT exportFormat = XFModel::filterToExportFormat(sSelectedFilter);

    if (!XFModel::exportToFile(pModel, exportFormat, sFileName)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot save file: %1").arg(sFileName));
    }
}
