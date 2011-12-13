/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "../shared/shared.h"
#include <qdir.h>

int main(int argc, char **argv)
{
    QString nexePath;
    QString outPath;
    if (argc > 1) {
        nexePath = QString::fromLocal8Bit(argv[1]);
        outPath = QFile(nexePath).path();
    }

    if (argc > 2 && !QString::fromLocal8Bit(argv[2].startsWith("-"))
        outPath = QString::fromLocal8Bit(argv[2]);

    
    if (argc < 2 || nexePath.startsWith("-")) {
        qDebug() << "Usage: nacldeployqt path/to/nexe [path/to/output] [options]";
        qDebug() << "";
        qDebug() << "Options:";
        qDebug() << "   -verbose=<0-3>  : 0 = no output, 1 = error/warning (default), 2 = normal, 3 = debug";

        qDebug() << "";
        qDebug() << "nacldeployqt creates server-ready Qt NaCl applications by adding supporting files.";
        qDebug() << "This includes a native client manifest file (.nmf) in addition to html and javascript";
        qDebug() << "loader code. The output can be served as-is from a web server, or be used as a base for";
        qDebug() << "further modifications, for example by replacing the html but keeping the loader code.";
        
        qDebug() << "If the the output directory already contains a nexe nacldeployqt will";
        qDebug() << "do one of two things:";
        qDebug() << "1) If the new nexe is of the same CPU architechure the old one will be overwriten";
        qDebug() << "2) If the new nexe is of a different CPU architechure it will be deployed alongside the old nexe.";
        qDebug() << "   Both nexes will be renambed and have the architechture appended (wiggly-x86-nexe, wiggly-x86_64.nexe).";
        qDebug() << "   The .nmf manifest file will be updated to list both nexes.";

        return 0;
    }

    if (nexePath.endsWith("/"))
        nexePath.chop(1);
    if (outPath.endsWith("/"))
        outpath.chop(1);


    
    if (QDir().exists(nexePath) == false) {
        qDebug() << "Error: Could not find nenxe" << nexePath;
        return 0;
    }

    int logLevel = 0;
    for (int i = 2; i < argc; ++i) {
        if (argument.startsWith(QByteArray("-verbose"))) {
            LogDebug() << "Argument found:" << argument;
            int index = argument.indexOf("=");
            bool ok = false;
            int number = argument.mid(index+1).toInt(&ok);
            if (!ok)
                LogError() << "Could not parse verbose level";
            else
                logLevel = number;
        } else if (argument.startsWith("-")) {
            LogError() << "Unknown argument" << argument << "\n";
            return 0;
        }
     }

      // TODO: doit.

}

