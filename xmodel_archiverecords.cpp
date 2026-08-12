/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to do so, subject to the
 * following conditions:
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
#include "xmodel_archiverecords.h"

namespace {

// Render a POSIX file mode as the familiar symbolic form plus octal, e.g.
// "-rwxr-xr-x (0755)".  A bare decimal mode value is unreadable.
QString modeToString(quint32 nMode, bool bIsFolder)
{
    QString sResult;

    quint32 nType = nMode & 0xF000u;  // S_IFMT

    if (nType == 0x4000u) sResult += QChar('d');       // S_IFDIR
    else if (nType == 0xA000u) sResult += QChar('l');  // S_IFLNK
    else if (nType == 0x8000u) sResult += QChar('-');  // S_IFREG
    else sResult += bIsFolder ? QChar('d') : QChar('-');

    const char cPerms[9] = {'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x'};

    for (qint32 i = 0; i < 9; i++) {
        sResult += (nMode & (1u << (8 - i))) ? QChar(cPerms[i]) : QChar('-');
    }

    return QString("%1 (0%2)").arg(sResult).arg(nMode & 0777u, 0, 8);
}

}  // namespace

XModel_ArchiveRecords::XModel_ArchiveRecords(const QList<XBinary::FPART_PROP> &listColumns, QList<XBinary::ARCHIVERECORD> *pListArchiveRecords, QObject *pParent)
    : XModel(pParent)
{
    m_listColumns = listColumns;

    m_pListArchiveRecords = pListArchiveRecords;
    if (pListArchiveRecords) {
        _setRowCount(pListArchiveRecords->count());
    } else {
        _setRowCount(0);
    }

    _setColumnCount(m_listColumns.count());
    _initColumns();
}

void XModel_ArchiveRecords::_initColumns()
{
    qint32 nNumberOfRecords = m_listColumns.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XBinary::FPART_PROP fpartProp = m_listColumns.at(i);

        QString sName;
        qint32 nFlags = Qt::AlignVCenter | Qt::AlignLeft;
        qint32 nSymbolSize = 16;

        if (fpartProp == XBinary::FPART_PROP_ORIGINALNAME) sName = QObject::tr("Name");
        else if (fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE) sName = QObject::tr("Compressed size");
        else if (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE) sName = QObject::tr("Size");
        else if (fpartProp == XBinary::FPART_PROP_STREAMOFFSET) sName = QObject::tr("Stream offset");
        else if (fpartProp == XBinary::FPART_PROP_STREAMSIZE) sName = QObject::tr("Stream size");
        else if (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD) sName = QObject::tr("Method");
        else if (fpartProp == XBinary::FPART_PROP_DATETIME) sName = QObject::tr("Modified");
        else if (fpartProp == XBinary::FPART_PROP_MTIME) sName = QObject::tr("Modified");
        else if (fpartProp == XBinary::FPART_PROP_CTIME) sName = QObject::tr("Created");
        else if (fpartProp == XBinary::FPART_PROP_ATIME) sName = QObject::tr("Accessed");
        else if (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDCRC) sName = QObject::tr("CRC");
        else if (fpartProp == XBinary::FPART_PROP_RESULTCRC) sName = QObject::tr("CRC");
        else if (fpartProp == XBinary::FPART_PROP_ENCRYPTED) sName = QObject::tr("Encrypted");
        else if (fpartProp == XBinary::FPART_PROP_FILEMODE) sName = QObject::tr("Mode");
        else if (fpartProp == XBinary::FPART_PROP_USERNAME) sName = QObject::tr("User");
        else if (fpartProp == XBinary::FPART_PROP_GROUPNAME) sName = QObject::tr("Group");
        else if (fpartProp == XBinary::FPART_PROP_UID) sName = QObject::tr("UID");
        else if (fpartProp == XBinary::FPART_PROP_GID) sName = QObject::tr("GID");
        else if (fpartProp == XBinary::FPART_PROP_LINKNAME) sName = QObject::tr("Link");
        else if (fpartProp == XBinary::FPART_PROP_INFO) sName = QObject::tr("Info");
        else if (fpartProp == XBinary::FPART_PROP_ISFOLDER) sName = QObject::tr("Folder");
        else if (fpartProp == XBinary::FPART_PROP_ISSOLID) sName = QObject::tr("Solid");
        else if (fpartProp == XBinary::FPART_PROP_SOLIDFOLDERINDEX) sName = QObject::tr("Solid block");
        else if (fpartProp == XBinary::FPART_PROP_CRC_TYPE) sName = QObject::tr("CRC type");
        else if (fpartProp == XBinary::FPART_PROP_FLAGS) sName = QObject::tr("Flags");
        else if (fpartProp == XBinary::FPART_PROP_TYPE) sName = QObject::tr("Type");
        else if (fpartProp == XBinary::FPART_PROP_FILETYPE) sName = QObject::tr("File type");
        else if (fpartProp == XBinary::FPART_PROP_EXT) sName = QObject::tr("Extension");
        else if (fpartProp == XBinary::FPART_PROP_WINDOWSIZE) sName = QObject::tr("Window size");
        else if (fpartProp == XBinary::FPART_PROP_STREAMUNPACKEDSIZE) sName = QObject::tr("Stream unpacked size");
        else if (fpartProp == XBinary::FPART_PROP_SUBSTREAMOFFSET) sName = QObject::tr("Substream offset");
        else if (fpartProp == XBinary::FPART_PROP_HEADER_OFFSET) sName = QObject::tr("Header offset");
        else if (fpartProp == XBinary::FPART_PROP_HEADER_SIZE) sName = QObject::tr("Header size");
        else if (fpartProp == XBinary::FPART_PROP_OPTHEADER_OFFSET) sName = QObject::tr("Opt header offset");
        else if (fpartProp == XBinary::FPART_PROP_OPTHEADER_SIZE) sName = QObject::tr("Opt header size");
        else if (fpartProp == XBinary::FPART_PROP_UUID) sName = QObject::tr("UUID");
        else if (fpartProp == XBinary::FPART_PROP_PREFIX) sName = QObject::tr("Prefix");
        else if (fpartProp == XBinary::FPART_PROP_DEVVERSION) sName = QObject::tr("Version");
        else if (fpartProp == XBinary::FPART_PROP_EXTRAFIELDOFFSET) sName = QObject::tr("Extra field offset");
        else if (fpartProp == XBinary::FPART_PROP_EXTRAFIELDLENGTH) sName = QObject::tr("Extra field length");
        else if (fpartProp == XBinary::FPART_PROP_FILECOMMENTOFFSET) sName = QObject::tr("Comment offset");
        else if (fpartProp == XBinary::FPART_PROP_FILECOMMENTLENGTH) sName = QObject::tr("Comment length");
        else if (fpartProp == XBinary::FPART_PROP_RESOURCEID) sName = QObject::tr("Resource ID");
        else if (fpartProp == XBinary::FPART_PROP_FILEMD5) sName = QObject::tr("File MD5");
        else if (fpartProp == XBinary::FPART_PROP_ISREADONLY) sName = QObject::tr("Read-only");
        else if (fpartProp == XBinary::FPART_PROP_ISHIDDEN) sName = QObject::tr("Hidden");
        else if (fpartProp == XBinary::FPART_PROP_ISSYSTEM) sName = QObject::tr("System");
        else if (fpartProp == XBinary::FPART_PROP_ISARCHIVE) sName = QObject::tr("Archive attribute");
        else if (fpartProp == XBinary::FPART_PROP_ISCOMMENTPRESENT) sName = QObject::tr("Has comment");
        else if (fpartProp == XBinary::FPART_PROP_COMPRESSPROPERTIES) sName = QObject::tr("Compress properties");
        else if (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD2) sName = QObject::tr("Method") + " 2";
        else if (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD3) sName = QObject::tr("Method") + " 3";
        else if (fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE2) sName = QObject::tr("Compressed size") + " 2";
        else if (fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE3) sName = QObject::tr("Compressed size") + " 3";
        else if (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE2) sName = QObject::tr("Size") + " 2";
        else if (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE3) sName = QObject::tr("Size") + " 3";

        if (sName.isEmpty()) sName = QString("#%1").arg(static_cast<qint32>(fpartProp));

        if ((fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE) || (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE) ||
            (fpartProp == XBinary::FPART_PROP_STREAMOFFSET) || (fpartProp == XBinary::FPART_PROP_STREAMSIZE) || (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDCRC) ||
            (fpartProp == XBinary::FPART_PROP_RESULTCRC) || (fpartProp == XBinary::FPART_PROP_UID) || (fpartProp == XBinary::FPART_PROP_GID))
            nFlags = Qt::AlignVCenter | Qt::AlignRight;
        else nFlags = Qt::AlignVCenter | Qt::AlignLeft;

        if (fpartProp == XBinary::FPART_PROP_ORIGINALNAME) nSymbolSize = 20;
        else if ((fpartProp == XBinary::FPART_PROP_DATETIME) || (fpartProp == XBinary::FPART_PROP_MTIME) || (fpartProp == XBinary::FPART_PROP_CTIME) ||
                 (fpartProp == XBinary::FPART_PROP_ATIME)) nSymbolSize = 19;
        else if (fpartProp == XBinary::FPART_PROP_ENCRYPTED) nSymbolSize = 9;
        else if ((fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDCRC) || (fpartProp == XBinary::FPART_PROP_RESULTCRC) || (fpartProp == XBinary::FPART_PROP_UID) ||
                 (fpartProp == XBinary::FPART_PROP_GID)) nSymbolSize = 10;
        else nSymbolSize = 16;

        setColumnName(i, sName);
        setColumnAlignment(i, nFlags);
        setColumnSymbolSize(i, nSymbolSize);
    }
}

QVariant XModel_ArchiveRecords::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!m_pListArchiveRecords) {
        return result;
    }

    if (index.isValid()) {
        qint32 nRow = index.row();
        if (nRow >= 0 && nRow < m_pListArchiveRecords->count()) {
            qint32 nColumn = index.column();
            const XBinary::ARCHIVERECORD &rec = m_pListArchiveRecords->at(nRow);
            if (nRole == Qt::DisplayRole) {
                if (nColumn < m_listColumns.count()) {
                    XBinary::FPART_PROP fpartProp = m_listColumns.at(nColumn);

                    if (fpartProp == XBinary::FPART_PROP_ORIGINALNAME) {
                        result = rec.mapProperties.value(fpartProp).toString();
                    } else if ((fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE) || (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE)) {
                        result = rec.mapProperties.value(fpartProp).toLongLong();
                    } else if (fpartProp == XBinary::FPART_PROP_STREAMOFFSET) {
                        result = rec.nStreamOffset;
                    } else if (fpartProp == XBinary::FPART_PROP_STREAMSIZE) {
                        result = rec.nStreamSize;
                    } else if (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD) {
                        result = XBinary::getHandleMethods(rec.mapProperties);
                    } else if ((fpartProp == XBinary::FPART_PROP_DATETIME) || (fpartProp == XBinary::FPART_PROP_MTIME) || (fpartProp == XBinary::FPART_PROP_CTIME) ||
                               (fpartProp == XBinary::FPART_PROP_ATIME)) {
                        QDateTime dt = rec.mapProperties.value(fpartProp).toDateTime();

                        if (dt.isValid()) {
                            // absolute times (7z FILETIME is UTC) shown in local zone;
                            // local/wall-clock times (DOS/ZIP) are unchanged by toLocalTime()
                            result = dt.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
                        }
                    } else if ((fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDCRC) || (fpartProp == XBinary::FPART_PROP_RESULTCRC)) {
                        if (rec.mapProperties.contains(fpartProp)) {
                            result = QString::number(rec.mapProperties.value(fpartProp).toULongLong(), 16).toUpper().rightJustified(8, QChar('0'));
                        }
                    } else if (fpartProp == XBinary::FPART_PROP_FILEMODE) {
                        if (rec.mapProperties.contains(fpartProp)) {
                            result = modeToString(rec.mapProperties.value(fpartProp).toUInt(),
                                                  rec.mapProperties.value(XBinary::FPART_PROP_ISFOLDER).toBool());
                        }
                    } else if (fpartProp == XBinary::FPART_PROP_ENCRYPTED) {
                        if (rec.mapProperties.value(fpartProp).toBool()) {
                            result = QObject::tr("Yes");
                        }
                    } else {
                        if (rec.mapProperties.contains(fpartProp)) {
                            QVariant varValue = rec.mapProperties.value(fpartProp);

                            if (varValue.userType() == QMetaType::Bool) {
                                result = varValue.toBool() ? QObject::tr("Yes") : QString();
                            } else if (varValue.userType() == QMetaType::QByteArray) {
                                result = QString(varValue.toByteArray().toHex());
                            } else if (varValue.userType() == QMetaType::QDateTime) {
                                result = varValue.toDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
                            } else {
                                result = varValue.toString();
                            }
                        }
                    }
                }

                // if (nColumn == COLUMN_NAME) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_ORIGINALNAME)) {
                //         result = rec.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
                //     }
                // } else if (nColumn == COLUMN_STREAMOFFSET) {
                //     result = QString::number(rec.nStreamOffset, 16);
                // } else if (nColumn == COLUMN_STREAMSIZE) {
                //     result = QString::number(rec.nStreamSize, 16);
                // // } else if (nColumn == COLUMN_DECOMPRESSEDOFFSET) {
                // //     result = QString::number(rec.nDecompressedOffset, 16);
                // // } else if (nColumn == COLUMN_DECOMPRESSEDSIZE) {
                // //     result = QString::number(rec.nDecompressedSize, 16);
                // } else if (nColumn == COLUMN_COMPRESSMETHOD) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_COMPRESSMETHOD)) {
                //         XBinary::COMPRESS_METHOD cm = (XBinary::COMPRESS_METHOD)rec.mapProperties.value(XBinary::FPART_PROP_COMPRESSMETHOD).toInt();
                //         result = XBinary::compressMethodToString(cm);
                //     }
                // } else if (nColumn == COLUMN_CRC) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_CRC_VALUE)) {
                //         quint32 nCRC = rec.mapProperties.value(XBinary::FPART_PROP_CRC_VALUE).toUInt();
                //         result = QString::number(nCRC, 16);
                //     }
                // } else if (nColumn == COLUMN_DATETIME) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_DATETIME)) {
                //         QDateTime dt = rec.mapProperties.value(XBinary::FPART_PROP_DATETIME).toDateTime();
                //         if (dt.isValid()) {
                //             result = dt.toString("yyyy-MM-dd HH:mm:ss");
                //         }
                //     }
                // }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            } else if (nRole >= Qt::UserRole) {
                if (nRole == (Qt::UserRole + XModel::USERROLE_ORIGINDEX)) {
                    result = nRow;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_OFFSET)) {
                    result = rec.nStreamOffset;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_SIZE)) {
                    result = rec.nStreamSize;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING1)) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_ORIGINALNAME)) {
                        result = rec.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
                    }
                }
            }
        }
    }

    return result;
}

QVariant XModel_ArchiveRecords::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (nRole == Qt::DisplayRole) {
            result = getColumnName(nSection);
        } else if (nRole == Qt::TextAlignmentRole) {
            result = getColumnAlignment(nSection);
        }
    }

    return result;
}
