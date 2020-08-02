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
#include "xdatetimeeditx.h"

XDateTimeEditX::XDateTimeEditX(QWidget *pParent): QDateTimeEdit(pParent)
{
    nValue=0;
    type=DT_TYPE_UNKNOWN;

    connect(this,SIGNAL(dateTimeChanged(const QDateTime &)),this,SLOT(_setDateTime(const QDateTime &)));
}

void XDateTimeEditX::setType(XDateTimeEditX::DT_TYPE type)
{
    this->type=type;

    if(type==DT_TYPE_POSIX)
    {
        setDisplayFormat("yyyy-MM-dd hh:mm:ss");
        QDateTime dt;
        dt=dt.fromString("1970-01-01 00:00:00","yyyy-MM-dd hh:mm:ss");
        setMinimumDateTime(QDateTime(dt));
        setDateTime(dt);
    }
}

void XDateTimeEditX::setValue(quint64 nValue)
{
    if(this->nValue!=nValue)
    {
        this->nValue=nValue;

        if(type==DT_TYPE_POSIX)
        {
            QDateTime dt;
            dt.setMSecsSinceEpoch((quint64)nValue*1000);

            setDateTime(dt);
        }

        emit valueChanged(nValue);
    }
}

quint64 XDateTimeEditX::getValue()
{
    return nValue;
}

void XDateTimeEditX::_setDateTime(const QDateTime &dt)
{
    quint64 nCurrentValue=0;

    if(type==DT_TYPE_POSIX)
    {
        nCurrentValue=(quint64)dt.toMSecsSinceEpoch()/1000;
    }

    if(nValue!=nCurrentValue)
    {
        nValue=nCurrentValue;

        emit valueChanged(nCurrentValue);
    }
}
