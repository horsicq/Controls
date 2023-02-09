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
#include "hexvalidator.h"

HEXValidator::HEXValidator(QObject *pParent) : QValidator(pParent)
{
    g_mode = MODE_HEX;
    g_nMax = 0xFFFFFFFFFFFFFFFF;
}

void HEXValidator::setData(HEXValidator::MODE mode, quint64 nMax)
{
    g_mode = mode;
    g_nMax = nMax;
}

quint64 HEXValidator::getMax()
{
    return g_nMax;
}

HEXValidator::MODE HEXValidator::getMode()
{
    return g_mode;
}

QValidator::State HEXValidator::validate(QString &sInput, int &nPos) const
{
    Q_UNUSED(nPos)

    QValidator::State result = Acceptable;

    if (!sInput.isEmpty()) {
        qint64 nSignMax = 0;
        qint64 nSignMin = 0;
        qint32 nHexLenght = 0;
        // TODO DecLenght

        // TODO optimize!
        if (g_nMax == 0xFF) {
            nSignMax = 128;
            nSignMin = -127;
            nHexLenght = 2;
        } else if (g_nMax == 0xFFFF) {
            nSignMax = 32767;
            nSignMin = -32768;
            nHexLenght = 4;
        } else if (g_nMax == 0xFFFFFFFF) {
            nSignMax = 2147483647;
            nSignMin = -2147483648;
            nHexLenght = 8;
        } else if (g_nMax == 0xFFFFFFFFFFFFFFFF) {
            nSignMax = 9223372036854775807;
            nSignMin = -9223372036854775808;
            nHexLenght = 16;
        }

        if (g_mode == MODE_HEX) {
            result = Invalid;

            bool bSuccess = false;
            quint64 nValue = sInput.toULongLong(&bSuccess, 16);

            if (bSuccess && (nValue <= g_nMax) && (sInput.length() <= nHexLenght)) {
                result = Acceptable;
            }
        } else if (g_mode == MODE_DEC) {
            result = Invalid;

            bool bSuccess = false;
            quint64 nValue = sInput.toULongLong(&bSuccess, 10);

            if (bSuccess && (nValue <= g_nMax)) {
                result = Acceptable;
            }
        } else if (g_mode == MODE_SIGN_DEC) {
            result = Invalid;

            bool bSuccess = false;
            qint64 nValue = sInput.toLongLong(&bSuccess, 10);

            if (bSuccess && (nValue <= nSignMax) && (nValue >= nSignMin)) {
                result = Acceptable;
            } else if (sInput == "-") {
                result = Intermediate;
            }
        }
        // TODO validate UUID !!!
        // NNNNNNNN-NNNN-NNNN-NNNN-NNNNNNNNNNNN
        // As 5 uints
    }

    return result;
}
