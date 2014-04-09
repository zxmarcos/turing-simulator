#include <QtWidgets>
#include <cmath>
#include "transitionitem.h"
#include "stateitem.h"
#include "automataview.h"
#include "machinescene.h"

using namespace std;


TransitionItem::TransitionItem(AutomataView *view, StateItem *from, StateItem *to) :
    QGraphicsItem(),
    m_selfTransition(from == to),
    m_from(from),
    m_to(to)
{
    m_start = mapFromItem(from, 0, 0);
    m_end = mapFromItem(to, 0, 0);
    m_to->addTransitionFrom(this);
    m_from->addTransitionTo(this);
    setZValue(-1);
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    m_isHovering = false;
    m_view = view;

    if (m_selfTransition) {
        m_control = QPointF(m_start.x(), m_start.y() - 100);
    } else
        m_control = QPointF((m_start.x() + m_end.x())/2, m_end.y());

    m_anchor = new TransitionAnchor(this, this);
    m_anchor->setPos(m_control);
    m_anchor->setRect(-8, -8, 16, 16);
    m_anchor->hide();

    m_accepts.clear();
}

TransitionItem::~TransitionItem()
{
    qDebug() << "Removendo transição " << m_from->getNumber() << "->" <<
                m_to->getNumber();
}

static inline double toRadian(double degree)
{
    return (degree * 0.0174532925d);
}

// rotaciona um ponto ao redor de outro ponto
QPointF rotateAroundPoint(QPointF p, QPointF o, double theta)
{
    const double px = p.x();
    const double py = p.y();
    const double ox = o.x();
    const double oy = o.y();

    return QPointF(cos(theta) * (px-ox) - sin(theta) * (py-oy) + ox,
                   sin(theta) * (px-ox) + cos(theta) * (py-oy) + oy);
}

void TransitionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_selfTransition) {
        drawSelf(painter);
    } else {
        drawNormal(painter);
    }


}

void TransitionItem::drawSelf(QPainter *painter)
{

    QLineF line(mapFromItem(m_from, m_from->boundingRect().center()),
                mapFromItem(m_anchor, m_anchor->boundingRect().center()));

    qreal length = line.length();
    QPointF edgeOffset((line.dx() * StateItem::radius) / length,
                       (line.dy() * StateItem::radius) / length);
    m_start = line.p1() + edgeOffset;
    m_end = line.p2();

    QPointF end = rotateAroundPoint(m_start, line.p1(), toRadian(-30));
    QPointF start = rotateAroundPoint(m_start, line.p1(), toRadian(+30));

    QPainterPath path;
    QPointF c1 = rotateAroundPoint(m_end, m_start, toRadian(+60));
    QPointF c2 = rotateAroundPoint(m_end, m_start, toRadian(-60));

    path.moveTo(start);
    path.cubicTo(c1, c2, end);

    m_end = end;

    if (isSelected()) {
        QPen pen(Qt::darkGreen);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawLine(m_start, m_control);
    }

    if (!m_isHovering)
        painter->setPen(Qt::black);
    else
        painter->setPen(Qt::blue);

    if (isSelected()) {
        QPen pen(painter->pen());
        pen.setColor(Qt::red);
        pen.setWidth(2);
        painter->setPen(pen);
    }

    drawArrow(painter, path, end);
    painter->drawPath(path);
    drawFunctions(painter, path);
}

void TransitionItem::drawNormal(QPainter *painter)
{
    QLineF line(mapFromItem(m_from, m_from->boundingRect().center()),
                mapFromItem(m_to, m_to->boundingRect().center()));
    qreal length = line.length();

    QPointF edgeOffset((line.dx() * StateItem::radius) / length,
                       (line.dy() * StateItem::radius) / length);
    m_start = line.p1() + edgeOffset;
    m_end = line.p2() - edgeOffset;

    line = QLineF(m_start, m_end);
    length = line.length();

    QPainterPath path;
    path.moveTo(m_start);
    path.quadTo(m_control, m_end);

    if (isSelected()) {
        QPen pen(Qt::darkGreen);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawLine(m_start, m_control);
        painter->drawLine(m_end, m_control);
    }

    if (!m_isHovering)
        painter->setPen(Qt::black);
    else
        painter->setPen(Qt::blue);

    if (isSelected()) {
        QPen p(painter->pen());
        p.setColor(Qt::red);
        p.setWidth(2);
        painter->setPen(p);
    }

    drawArrow(painter, path, m_end);
    painter->drawPath(path);
    drawFunctions(painter, path);
}

void TransitionItem::drawFunctions(QPainter *painter, QPainterPath &path)
{
    QFontMetrics metrics(painter->font());

    const int height = metrics.height();
    QPointF startPos(path.pointAtPercent(0.5) - QPointF(0, 5));

    int k = 0;
    foreach (AcceptState s, m_accepts) {
        QPointF at(startPos.x() - metrics.width(s.toString()) / 2,
                   startPos.y() - (height + 3) * k);
        painter->drawText(at, s.toString());
        k++;
    }
}

void TransitionItem::drawArrow(QPainter *painter, QPainterPath &path, QPointF origin)
{
    const int dx = 20;
    double length = path.length();
    const QPointF ref = path.pointAtPercent(path.percentAtLength((length - dx)));

    painter->drawLine(rotateAroundPoint(ref, origin, toRadian(-20)), m_end);
    painter->drawLine(rotateAroundPoint(ref, origin, toRadian(+20)), m_end);
}

void TransitionItem::shapeComplete(QPainterPath &path, QPointF origin) const
{
    const int dx = 20;
    double length = path.length();
    const QPointF ref = path.pointAtPercent(path.percentAtLength((length - dx)));



    QFont font;
    QFontMetrics metrics(font);
    const int height = metrics.height();
    QPointF startPos(path.pointAtPercent(0.5) - QPointF(0, 5));

    path.moveTo(rotateAroundPoint(ref, origin, toRadian(-20)));
    path.lineTo(origin);
    path.moveTo(rotateAroundPoint(ref, origin, toRadian(+20)));
    path.lineTo(origin);

    int k = 0;

    QPainterPath textRect;

    foreach (AcceptState s, m_accepts) {
        QPointF at(startPos.x() - metrics.width(s.toString()) / 2,
                   startPos.y() - (height + 3) * k);
        textRect.addText(at, font, s.toString());
        k++;
    }
    path.addRect(textRect.boundingRect());
}


QPainterPath TransitionItem::shape() const
{
    if (m_selfTransition) {
        return shapeSelf();
    } else {
        return shapeNormal();
    }

}

QPainterPath TransitionItem::shapeNormal() const
{
    QPointF m_start, m_end;
    QLineF line(mapFromItem(m_from, m_from->boundingRect().center()),
                mapFromItem(m_to, m_to->boundingRect().center()));
    qreal length = line.length();

    QPointF edgeOffset((line.dx() * StateItem::radius) / length,
                       (line.dy() * StateItem::radius) / length);
    m_start = line.p1() + edgeOffset;
    m_end = line.p2() - edgeOffset;

    line = QLineF(m_start, m_end);
    length = line.length();

    QPainterPath path;
    path.moveTo(m_start);
    path.quadTo(m_control, m_end);

    shapeComplete(path, m_end);
    return path;
}

QPainterPath TransitionItem::shapeSelf() const
{
    QPointF m_start, m_end;
    QLineF line(mapFromItem(m_from, m_from->boundingRect().center()),
                mapFromItem(m_anchor, m_anchor->boundingRect().center()));

    qreal length = line.length();
    QPointF edgeOffset((line.dx() * StateItem::radius) / length,
                       (line.dy() * StateItem::radius) / length);
    m_start = line.p1() + edgeOffset;
    m_end = line.p2();

    QPointF end = rotateAroundPoint(m_start, line.p1(), toRadian(-30));
    QPointF start = rotateAroundPoint(m_start, line.p1(), toRadian(+30));

    QPainterPath path;
    QPointF c1 = rotateAroundPoint(m_end, m_start, toRadian(+60));
    QPointF c2 = rotateAroundPoint(m_end, m_start, toRadian(-60));

    path.moveTo(start);
    path.cubicTo(c1, c2, end);

    shapeComplete(path, end);
    return path;
}

QList<AcceptState> TransitionItem::getAcceptList()
{
    return m_accepts;
}

void TransitionItem::setAcceptList(QList<AcceptState> &list)
{
    m_accepts = list;
}

QRectF TransitionItem::boundingRect() const
{
    return shape().boundingRect().adjusted(-5, -5, 5, 5);
}

StateItem *TransitionItem::getTo()
{
    return m_to;
}

StateItem *TransitionItem::getFrom()
{
    return m_from;
}

void TransitionItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    MachineScene *mach = reinterpret_cast<MachineScene*>(scene());
    if (mach->getMode() == MachineScene::Select) {
        scene()->clearSelection();
        setSelected(true);
        m_view->editTransition();
    }
}

QPointF TransitionItem::getControlPoint() const
{
    return m_control;
}

void TransitionItem::setControlPoint(QPointF point)
{
    if (m_anchor->pos() != point)
        m_anchor->setPos(point);
    m_control = point;
    update();
}

void TransitionItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_isHovering = true;
    update(boundingRect());
}

void TransitionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_isHovering = false;
    update(boundingRect());
}

QVariant TransitionItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange) {
        if (value.toBool())
            m_anchor->show();
        else m_anchor->hide();
        m_view->viewport()->update();
    }
    return QGraphicsItem::itemChange(change, value);
}

TransitionAnchor::TransitionAnchor(TransitionItem *item, QGraphicsItem *parent)
{
    Q_UNUSED(parent);
    m_transition = item;
    setFlag(ItemIsMovable);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemSendsGeometryChanges);
    setParentItem(item);

    setZValue(2);

    QPen pen(Qt::black, Qt::SolidLine);
    setPen(pen);
    setBrush(QColor(50, 0, 240, 50));
}

QVariant TransitionAnchor::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        m_transition->setControlPoint(pos());
        m_transition->getView()->viewport()->update();
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}
