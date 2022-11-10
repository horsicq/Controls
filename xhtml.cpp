/* Copyright (c) 2021-2022 hors<horsicq@gmail.com>
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
#include "xhtml.h"

XHtml::XHtml(QObject *pParent) : QObject(pParent) {
}

void XHtml::appendText(QString sText) {
    g_sString.append(sText);
}

void XHtml::addTableBegin() {
    g_sString.append(
        "<table border=\"1\" style=\"border-style:solid;\" cellspacing=\"-1\" "
        "cellpadding=\"5\">");  // TODO cellpadding consts
}

void XHtml::addTableRow(QList<TABLECELL> listTableCells) {
    qint32 nNumbersOfCell = listTableCells.count();

    if (nNumbersOfCell) {
        g_sString.append("<tr>");

        for (qint32 i = 0; i < nNumbersOfCell; i++) {
            g_sString.append("<td align=\"center\">");  // TODO align

            QString sCellText;

            if (listTableCells.at(i).bBold) {
                sCellText = QString("<b>%1</b>").arg(listTableCells.at(i).sText);
            } else {
                sCellText = listTableCells.at(i).sText;
            }

            g_sString.append(sCellText);
            g_sString.append("</td>");
        }

        g_sString.append("</tr>");
    }
}

void XHtml::addTableEnd() {
    g_sString.append("</table>");
}

QString XHtml::toString() {
    return g_sString;
}

QString XHtml::makeLink(QString sText, QString sLink) {
    QString sResult;

    if (sLink != "") {
        sResult = QString("<a href=\"%1\">%2</a>").arg(sLink, sText);
    } else {
        sResult = sText;
    }

    return sResult;
}
