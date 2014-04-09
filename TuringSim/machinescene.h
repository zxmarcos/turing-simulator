#ifndef MACHINESCENE_H
#define MACHINESCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QList>

class StateItem;
class AutomataView;

class MachineScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MachineScene(AutomataView *view, QObject *parent = 0);

    enum {
        Select = 0,
        InsertState,
        InsertTransition,
        DeleteItem,
    };
    int getMode() const;
    void setLock(bool locked);

    StateItem *getInitialState();
    QList<StateItem *> getFinalStates();
    StateItem *getActive();
    void setInativeAll();
    void setActive(StateItem *item);
    void setJam(StateItem *item);
public slots:
    void setMode(int mode);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    bool m_locked;
    int m_mode;
    AutomataView *m_view;
    QGraphicsLineItem *m_dragLine;
    QPointF m_startDragPoint;
    void createTransition(QGraphicsItem *from, QGraphicsItem *to);
    void deleteItem(QGraphicsItem *item);
};

#endif // MACHINESCENE_H
