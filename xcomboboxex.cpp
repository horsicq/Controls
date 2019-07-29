// copyright (c) 2017-2019 hors<horsicq@gmail.com>
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

XComboBoxEx::XComboBoxEx(QWidget *parent): QComboBox(parent)
{
    SubclassOfQStyledItemDelegate *delegate = new SubclassOfQStyledItemDelegate(this);
    setItemDelegate(delegate);

    nValue=0;
    bIsReadOnly=false;
    cbtype=CBTYPE_NORMAL;

    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedSlot(int)));
    connect(&model,SIGNAL(itemChanged(QStandardItem *)),this,SLOT(itemChangedSlot(QStandardItem *)));
}

void XComboBoxEx::setData(QMap<quint64,QString> mapData, CBTYPE cbtype, quint64 nMask)
{
    this->cbtype=cbtype;
    this->nMask=nMask;

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
        model.setItem(0,0,new QStandardItem("Flags"));
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
    this->nValue=nValue;

    if(cbtype==CBTYPE_NORMAL)
    {
        bool bFound=false;

        for(int i=1; i<model.rowCount(); i++)
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
    else if(cbtype==CBTYPE_EFLAGS)
    {
        bool bFound=false;

        for(int i=1; i<model.rowCount(); i++)
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
    else if(cbtype==CBTYPE_FLAGS)
    {
        for(int i=1; i<model.rowCount(); i++)
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
    return nValue;
}

void XComboBoxEx::setReadOnly(bool bIsReadOnly)
{
    this->bIsReadOnly=bIsReadOnly;

    if(cbtype==CBTYPE_FLAGS)
    {
        for(int i=0; i<model.rowCount(); i++)
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
    if(cbtype==CBTYPE_FLAGS)
    {
        if(nIndex)
        {
            setCurrentIndex(0);
        }
    }
    else if(cbtype==CBTYPE_EFLAGS)
    {
        if(!bIsReadOnly)
        {
            if(nIndex)
            {
                quint64 nCurrentValue=itemData(nIndex).toULongLong();

                quint64 _nValue=nValue;

                _nValue&=(~nMask);

                _nValue|=nCurrentValue;

                if(_nValue!=nValue)
                {
                    nValue=_nValue;
                    emit valueChanged(nValue);
                }
            }
        }
        else
        {
            // restore
            setValue(nValue);
        }
    }
    else if(cbtype==CBTYPE_NORMAL)
    {
        if(!bIsReadOnly)
        {
            if(nIndex)
            {
                quint64 nCurrentValue=itemData(nIndex).toULongLong();

                if(nCurrentValue!=nValue)
                {
                    nValue=nCurrentValue;
                    emit valueChanged(nValue);
                }
            }
        }
        else
        {
            // restore
            setValue(nValue);
        }
    }
}

void XComboBoxEx::itemChangedSlot(QStandardItem *item)
{
    Q_UNUSED(item)

    if((cbtype==CBTYPE_FLAGS)&&count())
    {
        quint64 nCurrentValue=nValue;

        for(int i=1; i<model.rowCount(); i++)
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

        if(nCurrentValue!=nValue)
        {
            nValue=nCurrentValue;
            emit valueChanged(nCurrentValue);
        }
    }
}
