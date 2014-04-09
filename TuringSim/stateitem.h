#ifndef STATEITEM_H
#define STATEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsView>
#include <QActionGroup>
#include <QObject>
#include <QPair>
#include "acceptstate.h"

class AutomataView;
class TransitionItem;
class MachineScene;
class StateItem;

struct TransitionAccept : public AcceptState {
    StateItem *next;
    TransitionAccept() : AcceptState() {
        next = nullptr;
    }

    TransitionAccept(StateItem *next_, AcceptState &s) :
        AcceptState(s) {
        next = next_;
    }
};

class StateItem : public QGraphicsItem
{
    friend class MachineScene;
public:
    explicit StateItem(AutomataView *parent = 0);
    virtual ~StateItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

    // adiciona items de transição
    void addTransitionFrom(TransitionItem *transition);
    void addTransitionTo(TransitionItem *transition);
    void removeTransition(TransitionItem *transition);
    void setNumber(int number);
    int getNumber() const;

    bool hasTransitionTo(StateItem *item);
    bool hasTransitionFrom(StateItem *item);

    TransitionAccept next(QString symbol);
    static const int radius = 30;

    typedef enum {
        Normal = 0,
        Initial,
        Final
    } StateType;

    typedef enum {
        Inative = 0,
        Active,
        Jammed
    } StatusType;

    StateType getType() const;
    void setType(StateType t);

    StatusType getStatus() const;
    void setStatus(StatusType type);

    static QString typeToString(StateType t);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
private:
    void deleteToTransitions();
    void deleteFromTransitions();
    void setActive(bool active);
    bool isActive();
    bool m_isHovering;
    StateType m_type;
    QList<TransitionItem *> m_transitionsTo;
    QList<TransitionItem *> m_transitionsFrom;
    AutomataView *m_view;
    int m_number;
    bool m_isActive;
    StatusType m_status;
};

#endif // STATEITEM_H
