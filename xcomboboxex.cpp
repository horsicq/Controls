/* Copyright (c) 2017-2026 hors<horsicq@gmail.com>
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
#include "xcomboboxex.h"

XComboBoxEx::XComboBoxEx(QWidget *pParent) : QComboBox(pParent)
{
    m_bIsReadonly = false;
    m_cbtype = CBTYPE_LIST;

    SubclassOfQStyledItemDelegate *pDelegate = new SubclassOfQStyledItemDelegate(this);
    setItemDelegate(pDelegate);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
    connect(&m_model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(itemChangedSlot(QStandardItem *)));
}

void XComboBoxEx::setData(const QMap<quint64, QString> &mapData, CBTYPE cbtype, quint64 nMask, const QString &sTitle)
{
    m_cbtype = cbtype;
    m_nMask = nMask;

    m_mapData = mapData;

    m_model.clear();
    m_model.setColumnCount(1);

    m_model.setRowCount(mapData.count() + 1);

    if ((cbtype == CBTYPE_LIST) || (cbtype == CBTYPE_ELIST)) {
        m_model.setItem(0, 0, new QStandardItem(""));  // Empty
    } else if ((cbtype == CBTYPE_FLAGS) || (cbtype == CBTYPE_CUSTOM_FLAGS)) {
        QString _sTitle = sTitle;

        if (_sTitle == "") {
            _sTitle = tr("Flags");
        }

        m_model.setItem(0, 0, new QStandardItem(_sTitle));
    }

    qint32 nIndex = 1;

    QMapIterator<quint64, QString> iter(mapData);

    while (iter.hasNext()) {
        iter.next();

        QStandardItem *pItem = new QStandardItem(iter.value());
        pItem->setData(iter.key(), Qt::UserRole);

        if (cbtype == CBTYPE_FLAGS) {
            pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            pItem->setData(Qt::Unchecked, Qt::CheckStateRole);
        }

        m_model.setItem(nIndex, 0, pItem);

        nIndex++;
    }

    setModel(&m_model);
}

void XComboBoxEx::setValue(QVariant varValue)
{
    this->m_varValue = varValue;

    qint32 nNumberOfItems = m_model.rowCount();

    if (m_cbtype == CBTYPE_LIST) {
        bool bFound = false;

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            quint64 _nValue = m_model.item(i, 0)->data(Qt::UserRole).toULongLong();

            if (_nValue == varValue) {
                setCurrentIndex(i);
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            setCurrentIndex(0);
        }
    } else if (m_cbtype == CBTYPE_ELIST) {
        bool bFound = false;
        quint64 nValue = varValue.toULongLong();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            quint64 _nValue = m_model.item(i, 0)->data(Qt::UserRole).toULongLong();
            nValue &= m_nMask;

            if (_nValue == nValue) {
                setCurrentIndex(i);
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            setCurrentIndex(0);
        }
    } else if (m_cbtype == CBTYPE_FLAGS) {
        quint64 nValue = varValue.toULongLong();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            quint64 _nValue = m_model.item(i, 0)->data(Qt::UserRole).toULongLong();

            if (_nValue & nValue) {
                m_model.item(i, 0)->setData(Qt::Checked, Qt::CheckStateRole);
            } else {
                m_model.item(i, 0)->setData(Qt::Unchecked, Qt::CheckStateRole);
            }
        }
    }
}

QVariant XComboBoxEx::getValue()
{
    return m_varValue;
}

void XComboBoxEx::setReadOnly(bool bIsReadOnly)
{
    this->m_bIsReadonly = bIsReadOnly;

    qint32 nNumberOfItems = m_model.rowCount();

    if (m_cbtype == CBTYPE_FLAGS) {
        for (qint32 i = 0; i < nNumberOfItems; i++) {
            if (bIsReadOnly) {
                m_model.item(i, 0)->setFlags(Qt::ItemIsEnabled);
            } else {
                m_model.item(i, 0)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            }
        }
    }
}

QString XComboBoxEx::getDescription()
{
    QString sResult;

    if (m_cbtype == CBTYPE_LIST) {
        sResult = m_mapData.value(m_varValue.toULongLong());
    } else if (m_cbtype == CBTYPE_ELIST) {
        sResult = m_mapData.value(m_varValue.toULongLong() & m_nMask);
    }
    if (m_cbtype == CBTYPE_FLAGS) {
        qint32 nNumberOfItems = m_model.rowCount();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            if (m_model.item(i, 0)->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                if (sResult != "") {
                    sResult += " | ";
                }

                sResult += m_mapData.value(m_model.item(i, 0)->data(Qt::UserRole).toULongLong());
            }
        }
    }

    return sResult;
}

void XComboBoxEx::addCustomFlags(const QString &sTitle, const QList<CUSTOM_FLAG> &listCustomFlags)
{
    m_model.clear();

    m_cbtype = CBTYPE_CUSTOM_FLAGS;

    m_model.setColumnCount(1);
    m_model.setItem(0, 0, new QStandardItem(sTitle));

    qint32 nNumberOfRecords = listCustomFlags.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        QStandardItem *pItem = new QStandardItem(listCustomFlags.at(i).sString);
        pItem->setData(listCustomFlags.at(i).nValue, Qt::UserRole);

        if (listCustomFlags.at(i).bIsReadOnly) {
            pItem->setFlags(Qt::ItemIsUserCheckable);
        } else {
            pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        }

        if (listCustomFlags.at(i).bIsChecked) {
            pItem->setData(Qt::Checked, Qt::CheckStateRole);
        } else {
            pItem->setData(Qt::Unchecked, Qt::CheckStateRole);
        }

        m_model.setItem(i + 1, 0, pItem);
    }

    setModel(&m_model);
}

void XComboBoxEx::setCustomFlag(quint64 nValue)
{
    qint64 nNumberOfRecords = m_model.rowCount();

    for (qint32 i = 1; i < nNumberOfRecords; i++) {
        if (m_model.item(i, 0)->data(Qt::UserRole).toULongLong() == nValue) {
            setItemData(i, Qt::Checked, Qt::CheckStateRole);
        }
    }
}

QList<quint64> XComboBoxEx::getCustomFlags()
{
    QList<quint64> listResult;

    qint32 nNumberOfRecords = m_model.rowCount();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (m_model.item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked) {
            quint64 nValue = m_model.item(i)->data(Qt::UserRole).toULongLong();

            listResult.append(nValue);
        }
    }

    return listResult;
}

void XComboBoxEx::_addCustomFlag(QList<CUSTOM_FLAG> *pListCustomFlags, quint64 nValue, const QString &sString, bool bChecked)
{
    CUSTOM_FLAG record = {};

    record.nValue = nValue;
    record.sString = sString;
    record.bIsChecked = bChecked;

    pListCustomFlags->append(record);
}

void XComboBoxEx::setItemEnabled(qint32 nRow, bool bState)
{
    m_model.item(nRow, 0)->setEnabled(bState);
}

void XComboBoxEx::currentIndexChangedSlot(int nIndex)
{
    if (m_cbtype == CBTYPE_FLAGS) {
        if (nIndex) {
            setCurrentIndex(0);
        }
    } else if (m_cbtype == CBTYPE_ELIST) {
        if (!m_bIsReadonly) {
            if (nIndex) {
                quint64 nCurrentValue = itemData(nIndex).toULongLong();

                quint64 _nValue = m_varValue.toULongLong();

                _nValue &= (~m_nMask);

                _nValue |= nCurrentValue;

                if (_nValue != m_varValue.toULongLong()) {
                    m_varValue = _nValue;
                    emit valueChanged(m_varValue);
                }
            }
        } else {
            // restore
            setValue(m_varValue);
        }
    } else if (m_cbtype == CBTYPE_LIST) {
        if (!m_bIsReadonly) {
            if (nIndex) {
                QVariant vCurrentValue = itemData(nIndex);

                if (vCurrentValue != m_varValue) {
                    m_varValue = vCurrentValue;
                    emit valueChanged(m_varValue);
                }
            }
        } else {
            // restore ild value
            setValue(m_varValue);
        }
    }
}

void XComboBoxEx::itemChangedSlot(QStandardItem *pItem)
{
    Q_UNUSED(pItem)

    if ((m_cbtype == CBTYPE_FLAGS) && count()) {
        quint64 nCurrentValue = m_varValue.toULongLong();

        qint32 nNumberOfItems = m_model.rowCount();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            if (m_model.item(i, 0)->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                nCurrentValue |= m_model.item(i, 0)->data(Qt::UserRole).toULongLong();
            } else {
                nCurrentValue &= (0xFFFFFFFFFFFFFFFF ^ m_model.item(i, 0)->data(Qt::UserRole).toULongLong());
            }
        }

        if (nCurrentValue != m_varValue.toULongLong()) {
            m_varValue = nCurrentValue;
            emit valueChanged(nCurrentValue);
        }
    }
}
