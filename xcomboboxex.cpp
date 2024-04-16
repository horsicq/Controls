/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
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
    g_bIsReadOnly = false;
    g_cbtype = CBTYPE_LIST;

    SubclassOfQStyledItemDelegate *pDelegate = new SubclassOfQStyledItemDelegate(this);
    setItemDelegate(pDelegate);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
    connect(&g_model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(itemChangedSlot(QStandardItem *)));
}

void XComboBoxEx::setData(QMap<quint64, QString> mapData, CBTYPE cbtype, quint64 nMask)
{
    g_cbtype = cbtype;
    g_nMask = nMask;

    g_mapData = mapData;

    g_model.clear();
    g_model.setColumnCount(1);

    g_model.setRowCount(mapData.count() + 1);

    if ((cbtype == CBTYPE_LIST) || (cbtype == CBTYPE_ELIST)) {
        g_model.setItem(0, 0, new QStandardItem(""));  // Empty
    } else if ((cbtype == CBTYPE_FLAGS) || (cbtype == CBTYPE_CUSTOM_FLAGS)) {
        g_model.setItem(0, 0, new QStandardItem(tr("Flags")));
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

        g_model.setItem(nIndex, 0, pItem);

        nIndex++;
    }

    setModel(&g_model);
}

void XComboBoxEx::setValue(QVariant vValue)
{
    this->g_vValue = vValue;

    qint32 nNumberOfItems = g_model.rowCount();

    if (g_cbtype == CBTYPE_LIST) {
        bool bFound = false;

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            quint64 _nValue = g_model.item(i, 0)->data(Qt::UserRole).toULongLong();

            if (_nValue == vValue) {
                setCurrentIndex(i);
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            setCurrentIndex(0);
        }
    } else if (g_cbtype == CBTYPE_ELIST) {
        bool bFound = false;
        quint64 nValue = vValue.toULongLong();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            quint64 _nValue = g_model.item(i, 0)->data(Qt::UserRole).toULongLong();
            nValue &= g_nMask;

            if (_nValue == nValue) {
                setCurrentIndex(i);
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            setCurrentIndex(0);
        }
    } else if (g_cbtype == CBTYPE_FLAGS) {
        quint64 nValue = vValue.toULongLong();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            quint64 _nValue = g_model.item(i, 0)->data(Qt::UserRole).toULongLong();

            if (_nValue & nValue) {
                g_model.item(i, 0)->setData(Qt::Checked, Qt::CheckStateRole);
            } else {
                g_model.item(i, 0)->setData(Qt::Unchecked, Qt::CheckStateRole);
            }
        }
    }
}

QVariant XComboBoxEx::getValue()
{
    return g_vValue;
}

void XComboBoxEx::setReadOnly(bool bIsReadOnly)
{
    this->g_bIsReadOnly = bIsReadOnly;

    qint32 nNumberOfItems = g_model.rowCount();

    if (g_cbtype == CBTYPE_FLAGS) {
        for (qint32 i = 0; i < nNumberOfItems; i++) {
            if (bIsReadOnly) {
                g_model.item(i, 0)->setFlags(Qt::ItemIsEnabled);
            } else {
                g_model.item(i, 0)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            }
        }
    }
}

QString XComboBoxEx::getDescription()
{
    QString sResult;

    if (g_cbtype == CBTYPE_LIST) {
        sResult = g_mapData.value(g_vValue.toULongLong());
    } else if (g_cbtype == CBTYPE_ELIST) {
        sResult = g_mapData.value(g_vValue.toULongLong());
    }
    if (g_cbtype == CBTYPE_FLAGS) {
        qint32 nNumberOfItems = g_model.rowCount();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            if (g_model.item(i, 0)->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                if (sResult != "") {
                    sResult += "|";
                }

                sResult += g_mapData.value(g_model.item(i, 0)->data(Qt::UserRole).toULongLong());
            }
        }
    }

    return sResult;
}

void XComboBoxEx::addCustomFlags(const QString &sTitle, const QList<CUSTOM_FLAG> &listCustomFlags)
{
    g_model.clear();

    g_cbtype = CBTYPE_CUSTOM_FLAGS;

    g_model.setColumnCount(1);
    g_model.setItem(0, 0, new QStandardItem(sTitle));

    qint32 nNumberOfRecords = listCustomFlags.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        QStandardItem *pItem = new QStandardItem(listCustomFlags.at(i).sString);
        pItem->setData(listCustomFlags.at(i).nValue, Qt::UserRole);

        pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

        if (listCustomFlags.at(i).bChecked) {
            pItem->setData(Qt::Checked, Qt::CheckStateRole);
        } else {
            pItem->setData(Qt::Unchecked, Qt::CheckStateRole);
        }

        g_model.setItem(i + 1, 0, pItem);
    }

    setModel(&g_model);
}

void XComboBoxEx::setCustomFlag(quint64 nValue)
{
    qint64 nNumberOfRecords = g_model.rowCount();

    for (qint32 i = 1; i < nNumberOfRecords; i++) {
        if (g_model.item(i, 0)->data(Qt::UserRole).toULongLong() == nValue) {
            setItemData(i, Qt::Checked, Qt::CheckStateRole);
        }
    }
}

QList<quint64> XComboBoxEx::getCustomFlags()
{
    QList<quint64> listResult;

    qint32 nNumberOfRecords = g_model.rowCount();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (g_model.item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked) {
            quint64 nValue = g_model.item(i)->data(Qt::UserRole).toULongLong();

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
    record.bChecked = bChecked;

    pListCustomFlags->append(record);
}

void XComboBoxEx::currentIndexChangedSlot(int nIndex)
{
    if (g_cbtype == CBTYPE_FLAGS) {
        if (nIndex) {
            setCurrentIndex(0);
        }
    } else if (g_cbtype == CBTYPE_ELIST) {
        if (!g_bIsReadOnly) {
            if (nIndex) {
                quint64 nCurrentValue = itemData(nIndex).toULongLong();

                quint64 _nValue = g_vValue.toULongLong();

                _nValue &= (~g_nMask);

                _nValue |= nCurrentValue;

                if (_nValue != g_vValue.toULongLong()) {
                    g_vValue = _nValue;
                    emit valueChanged(g_vValue);
                }
            }
        } else {
            // restore
            setValue(g_vValue);
        }
    } else if (g_cbtype == CBTYPE_LIST) {
        if (!g_bIsReadOnly) {
            if (nIndex) {
                QVariant vCurrentValue = itemData(nIndex);

                if (vCurrentValue != g_vValue) {
                    g_vValue = vCurrentValue;
                    emit valueChanged(g_vValue);
                }
            }
        } else {
            // restore ild value
            setValue(g_vValue);
        }
    }
}

void XComboBoxEx::itemChangedSlot(QStandardItem *pItem)
{
    Q_UNUSED(pItem)

    if ((g_cbtype == CBTYPE_FLAGS) && count()) {
        quint64 nCurrentValue = g_vValue.toULongLong();

        qint32 nNumberOfItems = g_model.rowCount();

        for (qint32 i = 1; i < nNumberOfItems; i++) {
            if (g_model.item(i, 0)->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                nCurrentValue |= g_model.item(i, 0)->data(Qt::UserRole).toULongLong();
            } else {
                nCurrentValue &= (0xFFFFFFFFFFFFFFFF ^ g_model.item(i, 0)->data(Qt::UserRole).toULongLong());
            }
        }

        if (nCurrentValue != g_vValue.toULongLong()) {
            g_vValue = nCurrentValue;
            emit valueChanged(nCurrentValue);
        }
    }
}
