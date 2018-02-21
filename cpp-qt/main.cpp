/**
 ** MIT License
 **
 ** This file is part of the MEMS-oriented-image-testing-technology project.
 ** Copyright (c) 2018 Lu <miroox@outlook.com>.
 **
 ** Permission is hereby granted, free of charge, to any person obtaining a copy
 ** of this software and associated documentation files (the "Software"), to deal
 ** in the Software without restriction, including without limitation the rights
 ** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 ** copies of the Software, and to permit persons to whom the Software is
 ** furnished to do so, subject to the following conditions:
 **
 ** The above copyright notice and this permission notice shall be included in all
 ** copies or substantial portions of the Software.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 ** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
 ** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 ** SOFTWARE.
 **/


#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QtDebug>
#include "mainpanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    // load translation
    QTranslator translator;
    QTranslator qtTranslator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    const QString& appTrPath = app.applicationDirPath() + "/translations";
    const QString& qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    for (QString locale : uiLanguages)
    {
        locale = QLocale(locale).name();
        if (translator.load(QLatin1String("mems_")+locale,appTrPath))
        {
            const QString& qtTrFile = QLatin1String("qt_") + locale;
            if (qtTranslator.load(qtTrFile,qtTrPath)
                || qtTranslator.load(qtTrFile,appTrPath))
            {
                app.installTranslator(&translator);
                app.installTranslator(&qtTranslator);
                qInfo() << "Language locale:" << locale;
                break;
            }
            translator.load(QString()); // unload
        }
        else if (locale == QLatin1String("C")
                   || locale.startsWith(QLatin1String("en")))
        {
            break; //english is built-in
        }
    }

    app.setApplicationDisplayName(QApplication::tr("MEMS oriented image testing technology"));
    MainPanel w;
    w.show();
    return app.exec();
}

