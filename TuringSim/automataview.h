#ifndef AUTOMATAVIEW_H
#define AUTOMATAVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QMenu>
#include "stateeditor.h"
#include "transitioneditor.h"
#include "machinedescriptor.h"

class TapeView;

class AutomataView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit AutomataView(TapeView *tape, QWidget *parent = 0);

    void editState();
    void editTransition();
    bool saveMachine(QString filename);
    bool loadMachine(QString filename);
    void clear();
    void setMachineComments(const QString &s);
    QString getMachineComments();
    void play(int time);
    void stop();
    bool isPlaying();
    int getStepCounter();
signals:
    void stepCounterChanged(int);
    void playStatusChanged(bool);
public slots:
    void resetMachine();
    void stepIn();
    void describe(QString comments);
protected:
    void wheelEvent(QWheelEvent *event);
    void timerEvent(QTimerEvent *event);
private:
    void lock(bool yes);
    int m_steps;
    QString m_machineComment;
    TapeView *m_tape;
    StateEditor *m_stateEditor;
    TransitionEditor *m_transitionEditor;
    QBasicTimer m_timer;
    MachineDescriptor *m_descriptor;
};

#endif // AUTOMATAVIEW_H
