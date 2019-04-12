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
#ifndef XCOMBOBOXEX_H
#define XCOMBOBOXEX_H

#include <QComboBox>
#include <QVariant>
#include <QHash>
#include <QStandardItemModel>
#include "subclassofqstyleditemdelegate.h"

class XComboBoxEx : public QComboBox
{
    Q_OBJECT
public:
    enum CBTYPE
    {
        CBTYPE_NORMAL=0,
        CBTYPE_FLAGS,
        CBTYPE_EFLAGS
    };

    explicit XComboBoxEx(QWidget *parent=nullptr);
    void setData(QMap<quint64,QString> mapData, CBTYPE cbtype=CBTYPE_NORMAL, quint64 nMask=0);
    void setValue(quint64 nValue);
    quint64 getValue();
    void setReadOnly(bool bIsReadOnly);

private slots:
    void currentIndexChangedSlot(int nIndex);
    void itemChangedSlot(QStandardItem *item);

signals:
    void valueChanged(quint64 nValue);

private:
    quint64 nValue;
    bool bIsReadOnly;
    QStandardItemModel model;
    CBTYPE cbtype;
    quint64 nMask;
};

#endif // XCOMBOBOXEX_H
