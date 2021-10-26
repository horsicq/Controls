// copyright (c) 2017-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "xcomboboxex.h"

XComboBoxEx::XComboBoxEx(QWidget *pParent): QComboBox(pParent)
{
    SubclassOfQStyledItemDelegate *pDelegate=new SubclassOfQStyledItemDelegate(this);
    setItemDelegate(pDelegate);

    g_nValue=0;
    g_bIsReadOnly=false;
    g_cbtype=CBTYPE_LIST;

    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedSlot(int)));
    connect(&g_model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(itemChangedSlot(QStandardItem*)));
}

void XComboBoxEx::setData(QMap<quint64,QString> mapData, CBTYPE cbtype, quint64 nMask)
{
    this->g_cbtype=cbtype;
    this->g_nMask=nMask;

    g_model.clear();
    g_model.setColumnCount(1);

    g_model.setRowCount(mapData.count());

    if((cbtype==CBTYPE_LIST)||(cbtype==CBTYPE_ELIST))
    {
        g_model.setItem(0,0,new QStandardItem("")); // Empty
    }
    else if(cbtype==CBTYPE_FLAGS)
    {
        g_model.setItem(0,0,new QStandardItem(tr("Flags")));
    }

    int nIndex=1;

    QMapIterator<quint64,QString> iter(mapData);

    while(iter.hasNext())
    {
        iter.next();

        QStandardItem* item=new QStandardItem(iter.value());
        item->setData(iter.key(),Qt::UserRole);

        if(cbtype==CBTYPE_FLAGS)
        {
            item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
            item->setData(Qt::Unchecked,Qt::CheckStateRole);
        }

        g_model.setItem(nIndex,0,item);

        nIndex++;
    }

    setModel(&g_model);
}

void XComboBoxEx::setValue(quint64 nValue)
{
    this->g_nValue=nValue;

    qint32 nNumberOfItems=g_model.rowCount();

    if(g_cbtype==CBTYPE_LIST)
    {
        bool bFound=false;

        for(qint32 i=1;i<nNumberOfItems;i++)
        {
            quint64 _nValue=g_model.item(i,0)->data(Qt::UserRole).toULongLong();

            if(_nValue==nValue)
            {
                setCurrentIndex(i);
                bFound=true;
                break;
            }
        }

        if(!bFound)
        {
            setCurrentIndex(0);
        }
    }
    else if(g_cbtype==CBTYPE_ELIST)
    {
        bool bFound=false;

        for(qint32 i=1;i<nNumberOfItems;i++)
        {
            quint64 _nValue=g_model.item(i,0)->data(Qt::UserRole).toULongLong();
            nValue&=g_nMask;

            if(_nValue==nValue)
            {
                setCurrentIndex(i);
                bFound=true;
                break;
            }
        }

        if(!bFound)
        {
            setCurrentIndex(0);
        }
    }
    else if(g_cbtype==CBTYPE_FLAGS)
    {
        for(qint32 i=1;i<nNumberOfItems;i++)
        {
            quint64 _nValue=g_model.item(i,0)->data(Qt::UserRole).toULongLong();

            if(_nValue&nValue)
            {
                g_model.item(i,0)->setData(Qt::Checked,Qt::CheckStateRole);
            }
            else
            {
                g_model.item(i,0)->setData(Qt::Unchecked,Qt::CheckStateRole);
            }
        }
    }
}

quint64 XComboBoxEx::getValue()
{
    return g_nValue;
}

void XComboBoxEx::setReadOnly(bool bIsReadOnly)
{
    this->g_bIsReadOnly=bIsReadOnly;

    qint32 nNumberOfItems=g_model.rowCount();

    if(g_cbtype==CBTYPE_FLAGS)
    {
        for(qint32 i=0;i<nNumberOfItems;i++)
        {
            if(bIsReadOnly)
            {
                g_model.item(i,0)->setFlags(Qt::ItemIsEnabled);
            }
            else
            {
                g_model.item(i,0)->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
            }
        }
    }
}

void XComboBoxEx::currentIndexChangedSlot(int nIndex)
{
    if(g_cbtype==CBTYPE_FLAGS)
    {
        if(nIndex)
        {
            setCurrentIndex(0);
        }
    }
    else if(g_cbtype==CBTYPE_ELIST)
    {
        if(!g_bIsReadOnly)
        {
            if(nIndex)
            {
                quint64 nCurrentValue=itemData(nIndex).toULongLong();

                quint64 _nValue=g_nValue;

                _nValue&=(~g_nMask);

                _nValue|=nCurrentValue;

                if(_nValue!=g_nValue)
                {
                    g_nValue=_nValue;
                    emit valueChanged(g_nValue);
                }
            }
        }
        else
        {
            // restore
            setValue(g_nValue);
        }
    }
    else if(g_cbtype==CBTYPE_LIST)
    {
        if(!g_bIsReadOnly)
        {
            if(nIndex)
            {
                quint64 nCurrentValue=itemData(nIndex).toULongLong();

                if(nCurrentValue!=g_nValue)
                {
                    g_nValue=nCurrentValue;
                    emit valueChanged(g_nValue);
                }
            }
        }
        else
        {
            // restore
            setValue(g_nValue);
        }
    }
}

void XComboBoxEx::itemChangedSlot(QStandardItem *pItem)
{
    Q_UNUSED(pItem)

    if((g_cbtype==CBTYPE_FLAGS)&&count())
    {
        quint64 nCurrentValue=g_nValue;

        int nNumberOfItems=g_model.rowCount();

        for(qint32 i=1;i<nNumberOfItems;i++)
        {
            if(g_model.item(i,0)->data(Qt::CheckStateRole).toInt()==Qt::Checked)
            {
                nCurrentValue|=g_model.item(i,0)->data(Qt::UserRole).toULongLong();
            }
            else
            {
                nCurrentValue&=(0xFFFFFFFFFFFFFFFF^g_model.item(i,0)->data(Qt::UserRole).toULongLong());
            }
        }

        if(nCurrentValue!=g_nValue)
        {
            g_nValue=nCurrentValue;
            emit valueChanged(nCurrentValue);
        }
    }
}
