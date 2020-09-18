// copyright (c) 2017-2020 hors<horsicq@gmail.com>
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
    SubclassOfQStyledItemDelegate *delegate=new SubclassOfQStyledItemDelegate(this);
    setItemDelegate(delegate);

    g_nValue=0;
    g_bIsReadOnly=false;
    g_cbtype=CBTYPE_NORMAL;

    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedSlot(int)));
    connect(&model,SIGNAL(itemChanged(QStandardItem *)),this,SLOT(itemChangedSlot(QStandardItem *)));
}

void XComboBoxEx::setData(QMap<quint64,QString> mapData, CBTYPE cbtype, quint64 nMask)
{
    this->g_cbtype=cbtype;
    this->g_nMask=nMask;

    QMapIterator<quint64,QString> iter(mapData);

    model.clear();
    model.setColumnCount(1);

    model.setRowCount(mapData.count());

    if((cbtype==CBTYPE_NORMAL)||(cbtype==CBTYPE_EFLAGS))
    {
        model.setItem(0,0,new QStandardItem("")); // Empty
    }
    else if(cbtype==CBTYPE_FLAGS)
    {
        model.setItem(0,0,new QStandardItem(tr("Flags")));
    }

    int nIndex=1;

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

        model.setItem(nIndex,0,item);

        nIndex++;
    }

    setModel(&model);
}

void XComboBoxEx::setValue(quint64 nValue)
{
    this->g_nValue=nValue;

    int nNumberOfItems=model.rowCount();

    if(g_cbtype==CBTYPE_NORMAL)
    {
        bool bFound=false;

        for(int i=1; i<nNumberOfItems; i++)
        {
            if(model.item(i,0)->data(Qt::UserRole).toULongLong()==nValue)
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
    else if(g_cbtype==CBTYPE_EFLAGS)
    {
        bool bFound=false;

        for(int i=1; i<nNumberOfItems; i++)
        {
            if(model.item(i,0)->data(Qt::UserRole).toULongLong()&nValue)
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
        for(int i=1; i<nNumberOfItems; i++)
        {
            if(model.item(i,0)->data(Qt::UserRole).toULongLong()&nValue)
            {
                model.item(i,0)->setData(Qt::Checked,Qt::CheckStateRole);
            }
            else
            {
                model.item(i,0)->setData(Qt::Unchecked,Qt::CheckStateRole);
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

    int nNumberOfItems=model.rowCount();

    if(g_cbtype==CBTYPE_FLAGS)
    {
        for(int i=0; i<nNumberOfItems; i++)
        {
            if(bIsReadOnly)
            {
                model.item(i,0)->setFlags(Qt::ItemIsEnabled);
            }
            else
            {
                model.item(i,0)->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
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
    else if(g_cbtype==CBTYPE_EFLAGS)
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
    else if(g_cbtype==CBTYPE_NORMAL)
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

        int nNumberOfItems=model.rowCount();

        for(int i=1; i<nNumberOfItems; i++)
        {
            if(model.item(i,0)->data(Qt::CheckStateRole).toInt()==Qt::Checked)
            {
                nCurrentValue|=model.item(i,0)->data(Qt::UserRole).toULongLong();
            }
            else
            {
                nCurrentValue&=(0xFFFFFFFFFFFFFFFF^model.item(i,0)->data(Qt::UserRole).toULongLong());
            }
        }

        if(nCurrentValue!=g_nValue)
        {
            g_nValue=nCurrentValue;
            emit valueChanged(nCurrentValue);
        }
    }
}
