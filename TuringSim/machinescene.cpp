#include <QtWidgets>
#include <QGraphicsScene>
#include "machinescene.h"
#include "stateitem.h"
#include "transitionitem.h"
#include "automataview.h"

MachineScene::MachineScene(AutomataView *view, QObject *parent) :
    QGraphicsScene(parent),
    m_view(view)
{
    m_mode = Select;
    setSceneRect(QRectF(-10000, -10000, 10000, 10000));
    m_dragLine = nullptr;
    m_locked = false;
}

int MachineScene::getMode() const
{
    return m_mode;
}

void MachineScene::setLock(bool locked)
{
    m_locked = locked;
}

StateItem *MachineScene::getInitialState()
{
    foreach (QGraphicsItem *item, items()) {
        StateItem *sitem = dynamic_cast<StateItem *>(item);
        if (sitem) {
            if (sitem->getType() == StateItem::Initial)
                return sitem;
        }
    }
    return nullptr;
}

QList<StateItem *> MachineScene::getFinalStates()
{
    QList<StateItem *> list;
    foreach (QGraphicsItem *item, items()) {
        StateItem *sitem = dynamic_cast<StateItem *>(item);
        if (sitem) {
            if (sitem->getType() == StateItem::Final)
                list.push_back(sitem);
        }
    }
    return list;
}

StateItem *MachineScene::getActive()
{
    foreach (QGraphicsItem *item, items()) {
        StateItem *sitem = dynamic_cast<StateItem *>(item);
        if (sitem) {
            if (sitem->getStatus() == StateItem::Active)
                return sitem;
        }
    }
    return nullptr;
}

void MachineScene::setInativeAll()
{
    foreach (QGraphicsItem *item, items()) {
        StateItem *sitem = dynamic_cast<StateItem *>(item);
        if (sitem) {
            sitem->setStatus(StateItem::Inative);
            sitem->update();
        }
    }
}

void MachineScene::setJam(StateItem *item)
{
    item->setStatus(StateItem::Jammed);
    item->update();
}


void MachineScene::setActive(StateItem *item)
{
    StateItem *last = getActive();
    if (last != nullptr) {
        last->setStatus(StateItem::Inative);
        last->update();
    }
    item->setStatus(StateItem::Active);
    item->update();
}

void MachineScene::setMode(int mode)
{
    if (mode == DeleteItem) {
        m_view->setCursor(Qt::CrossCursor);
    } else {
        m_view->setCursor(Qt::ArrowCursor);
    }

    m_mode = mode;
    clearSelection();
}

void MachineScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_locked)
        return;
    switch (m_mode) {
    case Select:
        QGraphicsScene::mousePressEvent(event);
        break;
    case InsertState: {
        StateItem *item = new StateItem(m_view);
        addItem(item);
        item->setPos(event->scenePos() - QPointF(item->boundingRect().width() / 2,
                                                 item->boundingRect().height() / 2));
        clearSelection();
        //item->setSelected(true);
        break;
    }
    case InsertTransition: {
        if (m_dragLine != nullptr) {
            delete m_dragLine;
            m_dragLine = nullptr;
            return;
        }
        m_dragLine = new QGraphicsLineItem();
        m_dragLine->setLine(QLineF(event->scenePos(), event->scenePos()));
        QPen pen(Qt::black);
        pen.setStyle(Qt::DashLine);
        m_dragLine->setPen(pen);
        m_startDragPoint = event->scenePos();
        addItem(m_dragLine);
        break;
    }
    default:
        break;
    }
}

void MachineScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_locked)
        return;
    switch (m_mode) {
    case Select:
    case DeleteItem:
        QGraphicsScene::mouseMoveEvent(event);
        break;
    case InsertTransition: {
        if (m_dragLine != nullptr) {
            m_dragLine->setLine(QLineF(m_startDragPoint, event->scenePos()));
        }
        break;
    }
    default:
        break;
    }
}

void MachineScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_locked)
        return;
    switch (m_mode) {
    case Select:
        QGraphicsScene::mouseReleaseEvent(event);
        break;
    case DeleteItem:
        deleteItem(itemAt(event->scenePos(), QTransform()));
        break;
    case InsertTransition: {
        if (m_dragLine != nullptr) {
            delete m_dragLine;
            m_dragLine = nullptr;
            QGraphicsItem *srcItem = itemAt(m_startDragPoint, QTransform());
            QGraphicsItem *dstItem = itemAt(event->scenePos(), QTransform());
            if ((srcItem != nullptr) && (dstItem != nullptr)) {
                createTransition(srcItem, dstItem);
            }
        }
    }
    default:
        break;
    }
}

void MachineScene::createTransition(QGraphicsItem *from, QGraphicsItem *to)
{
    StateItem *src = dynamic_cast<StateItem *>(from);
    StateItem *dst = dynamic_cast<StateItem *>(to);
    if ((src == nullptr) || (dst == nullptr))
        return;
    if (src->hasTransitionTo(dst)) {
        QMessageBox::warning(m_view, "Erro", "Já existe essa transição!\nEdite a transição existente.");
        return;
    }
    qDebug() << "Criando transição";
    TransitionItem *trans = new TransitionItem(m_view, src, dst);
    addItem(trans);
    clearSelection();
    //trans->setSelected(true);
}

void MachineScene::deleteItem(QGraphicsItem *item)
{
    if (dynamic_cast<TransitionItem *>(item) != nullptr) {
        TransitionItem *trans = static_cast<TransitionItem *>(item);
        trans->getTo()->removeTransition(trans);
        trans->getFrom()->removeTransition(trans);
        delete trans;
    } else if (dynamic_cast<StateItem *>(item) != nullptr) {
        StateItem *state = static_cast<StateItem *>(item);
        state->deleteFromTransitions();
        state->deleteToTransitions();
        delete state;
    }
}
