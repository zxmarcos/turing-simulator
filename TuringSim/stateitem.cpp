#include <QtWidgets>
#include "stateitem.h"
#include "transitionitem.h"
#include "automataview.h"
#include "machinescene.h"

static int counter = 0;

StateItem::StateItem(AutomataView *parent) :
    QGraphicsItem(NULL)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setZValue(0);
    m_view = parent;
    m_number = counter++;
    m_type = Normal;
    m_isHovering = false;
    m_status = Inative;
}

StateItem::~StateItem()
{
    qDebug() << "Deletando estado " << m_number;
}

QRectF StateItem::boundingRect() const
{
    return QRectF(0, 0, radius * 2, radius * 2);
}

void StateItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    QPen pen(Qt::black, 1);
    if (m_isHovering)
        pen.setColor(Qt::blue);
    if (isSelected()) {
        pen.setColor(Qt::red);
        pen.setWidth(2);
    }
    if (m_status == Active) {
        if (m_type == Final)
            painter->setBrush(QColor(200,255,200));
        else painter->setBrush(QColor(255,255,200));

    } else if (m_status == Jammed)
        painter->setBrush(QColor(255, 200, 200));

    painter->setPen(pen);
    painter->drawEllipse(boundingRect());

    // se for estado inicial ou final
    if (m_type == Final) {
        const int inner = 5;
        painter->drawEllipse(boundingRect().adjusted(inner, inner, -inner, -inner));
    }
    painter->drawText(boundingRect(), Qt::AlignCenter,
                      QString::number(m_number));

    // desenha seta inicial
    if (m_type == Initial) {
        painter->setBrush(Qt::red);
        QPolygonF polygon;
        QPointF middle(boundingRect().center());
        middle.setX(middle.x() - (boundingRect().height() * 0.3));

        QPointF up(boundingRect().topLeft());
        up.setY(up.y() + (boundingRect().height() * 0.3));

        QPointF down(boundingRect().bottomLeft());
        down.setY(down.y() - (boundingRect().height() * 0.3));

        polygon << up << down << middle;
        painter->drawPolygon(polygon);
    }

}

QPainterPath StateItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void StateItem::addTransitionFrom(TransitionItem *transition)
{
    m_transitionsFrom << transition;
}

void StateItem::addTransitionTo(TransitionItem *transition)
{
    m_transitionsTo << transition;
}

void StateItem::removeTransition(TransitionItem *transition)
{
    m_transitionsFrom.removeAll(transition);
    m_transitionsTo.removeAll(transition);
}

void StateItem::setNumber(int number)
{
    m_number = number;
}

int StateItem::getNumber() const
{
    return m_number;
}

bool StateItem::hasTransitionTo(StateItem *item)
{
    foreach (TransitionItem *i, m_transitionsTo) {
        if (i->getTo() == item)
            return true;
    }
    return false;
}

bool StateItem::hasTransitionFrom(StateItem *item)
{
    foreach (TransitionItem *i, m_transitionsFrom) {
        if (i->getTo() == item)
            return true;
    }
    return false;
}

TransitionAccept StateItem::next(QString symbol)
{
    foreach (TransitionItem *trans, m_transitionsTo) {
        QList<AcceptState> accepts = trans->getAcceptList();
        foreach (AcceptState t, accepts) {
            //qDebug() << "Comparando: " << symbol << " com " << t.accept;
            if (!(t.accept.compare(symbol, Qt::CaseSensitive)))
                return TransitionAccept(trans->getTo(), t);
        }
    }
    return TransitionAccept();
}


StateItem::StateType StateItem::getType() const
{
    return m_type;
}

void StateItem::setType(StateItem::StateType t)
{
    m_type = t;
}

StateItem::StatusType StateItem::getStatus() const
{
    return m_status;
}

void StateItem::setStatus(StateItem::StatusType type)
{
    if (m_status != type)
        m_status = type;
}

QString StateItem::typeToString(StateItem::StateType t)
{
    switch (t) {
    case Normal: return "normal";
    case Final: return "final";
    case Initial: return "initial";
    }
    return "normal";
}

QVariant StateItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                               const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        update();
        foreach(TransitionItem *item, m_transitionsFrom)
            item->update();
        foreach(TransitionItem *item, m_transitionsFrom)
            item->update();
        if (m_view)
            m_view->viewport()->update();
    }
    return QGraphicsItem::itemChange(change, value);
}

void StateItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    MachineScene *mach = reinterpret_cast<MachineScene*>(scene());
    if (mach->getMode() == MachineScene::Select) {
        scene()->clearSelection();
        setSelected(true);
        m_view->editState();
    }
}

void StateItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_isHovering = true;
    update(boundingRect());
}

void StateItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_isHovering = false;
    update(boundingRect());
}

void StateItem::deleteToTransitions()
{
    foreach (TransitionItem *item, m_transitionsTo) {
        item->getTo()->removeTransition(item);
        delete item;
    }
    m_transitionsTo.clear();
}

void StateItem::deleteFromTransitions()
{
    foreach (TransitionItem *item, m_transitionsFrom) {
        item->getFrom()->removeTransition(item);
        delete item;
    }
    m_transitionsFrom.clear();
}
