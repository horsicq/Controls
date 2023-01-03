/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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
#ifndef HEXVALIDATOR_H
#define HEXVALIDATOR_H

#include <QIntValidator>

class HEXValidator : public QValidator {
    Q_OBJECT

public:
    enum MODE {
        MODE_TEXT = 0,
        MODE_DEC,
        MODE_SIGN_DEC,
        MODE_HEX,
        MODE_UUID
    };

    explicit HEXValidator(QObject *pParent = nullptr);

    void setData(MODE mode, quint64 nMax);
    MODE getMode();
    virtual State validate(QString &sInput, int &nPos) const;

private:
    MODE g_mode;
    quint64 g_nMax;
};

#endif  // HEXVALIDATOR_H
