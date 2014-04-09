#ifndef MACHINEDESCRIPTOR_H
#define MACHINEDESCRIPTOR_H

#include <QDialog>
#include <QTextBrowser>
#include <QToolBar>
#include <QAction>
#include "transitionitem.h"
#include "stateitem.h"
#include "machinescene.h"

class MachineDescriptor : public QDialog
{
    Q_OBJECT
public:
    explicit MachineDescriptor(QWidget *parent = 0);
    void setMachineScene(MachineScene *scene);
    void setMachineComments(QString str);
signals:
public slots:
    void print();
private:
    QTextBrowser *m_editor;
    QToolBar *m_toolbar;
    QAction *m_export;
    QString m_comments;
};

#endif // MACHINEDESCRIPTOR_H
