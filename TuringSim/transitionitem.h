#ifndef TRANSITIONITEM_H
#define TRANSITIONITEM_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QPointF>
#include <QGraphicsView>
#include <QString>
#include "acceptstate.h"

class AutomataView;
class StateItem;



class TransitionItem;
class TransitionAnchor : public QGraphicsEllipseItem
{
public:
    TransitionAnchor(TransitionItem *item, QGraphicsItem *parent = 0);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    TransitionItem *m_transition;
};

class TransitionItem : public QGraphicsItem
{
    friend class TransitionAnchor;
public:
    TransitionItem(AutomataView *view, StateItem *from, StateItem *to);
    virtual ~TransitionItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QList<AcceptState> getAcceptList();
    void setAcceptList(QList<AcceptState> &list);
    QRectF boundingRect() const;

    StateItem *getTo();
    StateItem *getFrom();
    QPointF getControlPoint() const;
    void setControlPoint(QPointF point);
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    void shapeComplete(QPainterPath &path, QPointF origin) const;
    QPainterPath shapeSelf() const;
    QPainterPath shapeNormal() const;
    void drawSelf(QPainter *painter);
    void drawNormal(QPainter *painter);
    void drawFunctions(QPainter *painter, QPainterPath &path);
    void drawArrow(QPainter *painter, QPainterPath &path, QPointF origin);
    const bool m_selfTransition;
    bool m_isHovering;
    QPointF m_start;
    QPointF m_end;
    QPointF m_control;
    StateItem *m_from;
    StateItem *m_to;
    AutomataView *m_view;
    TransitionAnchor *m_anchor;
    QList<AcceptState> m_accepts;
public:
    AutomataView *getView() {
        return m_view;
    };
};


#endif // TRANSITIONITEM_H
