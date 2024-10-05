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
#include "xheaderview.h"

XHeaderView::XHeaderView(QWidget *pParent) : QHeaderView(Qt::Horizontal, pParent)
{
    connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized(int, int, int)));
}

QSize XHeaderView::sizeHint() const
{
    QSize baseSize = QHeaderView::sizeHint();

    if (g_listLineEdits.count()) {
        baseSize.setHeight(baseSize.height() + 4 + g_listLineEdits.at(0)->sizeHint().height());
    }

    return baseSize;
}

void XHeaderView::setNumberOfFilters(qint32 nNumberOfFilters)
{
    qint32 nOldCount = g_listLineEdits.count();

    for (qint32 i = 0; i < nOldCount; i++) {
        delete g_listLineEdits.at(i);
    }

    g_listLineEdits.clear();

    for (qint32 i = 0; i < nNumberOfFilters; i++) {
        QLineEdit *pLineEdit = new QLineEdit(this);
        pLineEdit->setPlaceholderText(tr("Filter"));
        connect(pLineEdit, SIGNAL(textChanged(QString)), this, SLOT(_textChanged(QString)));
        g_listLineEdits.append(pLineEdit);
    }

    adjustPositions();
}

void XHeaderView::updateGeometries()
{
    if (g_listLineEdits.count()) {
        qint32 nHeight = g_listLineEdits.at(0)->sizeHint().height();
        setViewportMargins(0, 0, 0, nHeight + 4);
    } else {
        setViewportMargins(0, 0, 0, 0);
    }

    QHeaderView::updateGeometries();
    adjustPositions();
}

void XHeaderView::adjustPositions()
{
    qint32 nCount = g_listLineEdits.count();

    QSize baseSize = QHeaderView::sizeHint();

    for (qint32 i = 0; i < nCount; i++) {
        qint32 nHeight = g_listLineEdits.at(i)->sizeHint().height();
        g_listLineEdits.at(i)->move(sectionPosition(i) - offset(), baseSize.height() + 2);
        g_listLineEdits.at(i)->resize(sectionSize(i) - 2, nHeight);
    }
}

QList<QString> XHeaderView::getFilters()
{
    QList<QString> listResult;

    qint32 nCount = g_listLineEdits.count();

    for (qint32 i = 0; i < nCount; i++) {
        listResult.append(g_listLineEdits.at(i)->text());
    }

    return listResult;
}

void XHeaderView::_textChanged(QString sText)
{
    Q_UNUSED(sText)

    emit filterChanged();
}

void XHeaderView::onSectionResized(int i, int nOldSize, int nNewSize)
{
    Q_UNUSED(i)
    Q_UNUSED(nOldSize)
    Q_UNUSED(nNewSize)

    adjustPositions();
}
