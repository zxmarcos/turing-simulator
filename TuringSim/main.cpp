#include <QApplication>
#include <QtWidgets>
#include "mainwindow.h"
#include "stateeditor.h"
#include "transitioneditor.h"
#include "machinedescriptor.h"

QString loadAppStyle(const QString &name)
{
    QFile file(name);
    file.open(QFile::ReadOnly);

    return QString(file.readAll());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle("fusion");
    app.setStyleSheet(loadAppStyle(":/styler.css"));
#if 1
    MainWindow w;
    w.show();
#else
    MachineDescriptor desc;
    desc.exec();
#endif
    return app.exec();
}
