/* Copyright (c) 2017-2021 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef XDATETIMEEDITX_H
#define XDATETIMEEDITX_H

#include <QDateTimeEdit>

class XDateTimeEditX : public QDateTimeEdit
{
    Q_OBJECT

public:
    enum DT_TYPE
    {
        DT_TYPE_UNKNOWN=0,
        DT_TYPE_POSIX
        // TODO More
    };

    XDateTimeEditX(QWidget *pParent=nullptr);
    void setType(DT_TYPE dtType);
    void setValue(quint64 nValue);
    quint64 getValue();

private slots:
    void _setDateTime(const QDateTime &dt);

signals:
    void valueChanged(quint64 nValue);

private:
    quint64 g_nValue;
    DT_TYPE g_dtType;
};

#endif // XDATETIMEEDITX_H
