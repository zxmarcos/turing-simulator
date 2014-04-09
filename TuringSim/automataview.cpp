#include <QtWidgets>
#include <QtXml>
#include "machinescene.h"
#include "automataview.h"
#include "stateeditor.h"
#include "stateitem.h"
#include "transitioneditor.h"
#include "tapeview.h"

AutomataView::AutomataView(TapeView *tape, QWidget *parent) :
    QGraphicsView(parent),
    m_tape(tape)
{
    setRenderHints((QPainter::Antialiasing | QPainter::SmoothPixmapTransform));
    m_stateEditor = new StateEditor(this);
    m_transitionEditor = new TransitionEditor(this);
    m_descriptor = new MachineDescriptor(this);
}

void AutomataView::editState()
{
    StateItem *item = nullptr;
    item = dynamic_cast<StateItem *>(scene()->selectedItems().at(0));
    if (item == nullptr) {
        qDebug() << "Item inválido!!!";
        return;
    }
    m_stateEditor->setState(item);
    if (m_stateEditor->exec() == QDialog::Accepted) {
        item->setNumber(m_stateEditor->getNumber());

        // só podemos ter um estado inicial na nossa máquina
        if (m_stateEditor->getType() == StateItem::Initial) {
            MachineScene *mscene = reinterpret_cast<MachineScene*>(scene());
            StateItem *currentInitial = mscene->getInitialState();
            if (currentInitial != nullptr) {
                currentInitial->setType(StateItem::Normal);
                currentInitial->update();
            }
        }
        item->setToolTip(m_stateEditor->getComment());
        item->setType(m_stateEditor->getType());
        item->update();
    }
}

void AutomataView::editTransition()
{
    TransitionItem *item = nullptr;
    item = dynamic_cast<TransitionItem *>(scene()->selectedItems().at(0));
    if (item == nullptr) {
        qDebug() << "Item inválido!!!";
        return;
    }
    m_transitionEditor->setComment(item->toolTip());
    m_transitionEditor->setAcceptList(item->getAcceptList());
    if (m_transitionEditor->exec() == QDialog::Accepted) {
        QList<AcceptState> tl = m_transitionEditor->getAcceptList();
        item->setAcceptList(tl);
        item->setToolTip(m_transitionEditor->getComment());
        item->update();
    }
}

bool AutomataView::saveMachine(QString filename)
{

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Erro", "Não foi possível criar o arquivo!");
        return false;
    }

    QList<QGraphicsItem*> items = scene()->items();
    // tabela que mapeará um StateItem* para um indice.
    QMap<StateItem *, int> stateMap;
    QList<TransitionItem *> transitions;

    // vamos criar as tabelas e listas
    int stateItems = 0;

    foreach (QGraphicsItem *current, items) {
        if (dynamic_cast<StateItem *>(current) != nullptr) {
            stateMap[reinterpret_cast<StateItem *>(current)] = stateItems;
            stateItems++;
        } else if (dynamic_cast<TransitionItem *>(current) != nullptr) {
            transitions.push_back(reinterpret_cast<TransitionItem*>(current));
        }
    }
    // se não tivermos nenhum elemento, então não gravamos nada.
    if (stateItems == 0) {
        QMessageBox::critical(this, "Erro", "A máquina está vazia!");
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("turing-machine");
    if (!m_machineComment.isEmpty()) {
        writer.writeStartElement("comment");
        writer.writeCDATA(m_machineComment);
        writer.writeEndElement();
    }

    // descreve os estados da máquina
    writer.writeStartElement("machine-states");
    foreach (StateItem *item, stateMap.keys()) {
        writer.writeStartElement("state");
        writer.writeAttribute("id", QString::number(stateMap[item]));
        writer.writeAttribute("name", QString::number(item->getNumber()));
        writer.writeAttribute("x", QString::number(item->x()));
        writer.writeAttribute("y", QString::number(item->y()));
        writer.writeAttribute("type", StateItem::typeToString(item->getType()));
        if (!item->toolTip().isEmpty()) {
            writer.writeStartElement("comment");
            writer.writeCDATA(item->toolTip());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }
    writer.writeEndElement();

    // descreve as transições da máquina
    writer.writeStartElement("state-transitions");
    foreach (TransitionItem *item, transitions) {
        writer.writeStartElement("transition");
        writer.writeAttribute("from", QString::number(stateMap[item->getFrom()]));
        writer.writeAttribute("to", QString::number(stateMap[item->getTo()]));
        writer.writeAttribute("x", QString::number(item->getControlPoint().x()));
        writer.writeAttribute("y", QString::number(item->getControlPoint().y()));
        if (!item->toolTip().isEmpty()) {
            writer.writeStartElement("comment");
            writer.writeCDATA(item->toolTip());
            writer.writeEndElement();
        }

        foreach (AcceptState acc, item->getAcceptList()) {
            writer.writeStartElement("function");
            writer.writeAttribute("read", acc.accept);
            writer.writeAttribute("write", acc.write);
            writer.writeAttribute("move", acc.dirToString());
            writer.writeEndElement();
        }

        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndDocument();
    return true;
}

bool AutomataView::loadMachine(QString filename)
{
    const double xoffset = 0;
    const double yoffset = 0;
    // função utilitária para carregar comentários
    auto getComments = [](QDomElement &node) -> QString {
        QDomNode entry = node.firstChildElement("comment");
        if (!entry.isNull()) {
            return entry.toElement().text();
        }
        return QString();
    };

    qDebug() << "Tentando carregar: " << filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erro", "Não foi possível abrir o arquivo!");
        return false;
    }

    QDomDocument dom;
    if (!dom.setContent(&file)) {
        QMessageBox::critical(this, "Erro", "Erro ao fazer o parse do arquivo!");
        return false;
    }

    QDomElement root = dom.documentElement();
    if (root.nodeName() != "turing-machine") {
        QMessageBox::critical(this, "Erro", "O arquivo não descreve uma máquina de Turing!");
        return false;
    }

    // limpa a cena
    scene()->clear();

    m_machineComment = getComments(root);

    QDomNodeList stateList = dom.elementsByTagName("machine-states");
    qDebug() << stateList.count() << " estados!";
    if (stateList.count() == 0) {
        QMessageBox::critical(this, "Erro", "Não existe nenhum estado no arquivo!");
        return false;
    }

    // mapeia um Id para um StateItem*
    QMap<int, StateItem *> stateMap;

    int initialStates = 0;
    for (int i = 0; i < stateList.count(); i++) {
        QDomElement rootElement = stateList.at(i).toElement();

        QDomNode entry = rootElement.firstChildElement("state");
        while (!entry.isNull()) {
            QDomElement elem = entry.toElement();
            StateItem *state = new StateItem(this);

            state->setToolTip(getComments(elem));

            // grava o id desse estado
            stateMap[elem.attribute("id").toInt()] = state;
#if 0
            qDebug() << "Estado: " << elem.attribute("name").toInt() << ", " <<
                        elem.attribute("type") << ", " << elem.attribute("x").toDouble() <<
                        elem.attribute("y").toDouble();
#endif

            // define o tipo do estado
            state->setNumber(elem.attribute("name").toInt());
            if (elem.attribute("type") == "final")
                state->setType(StateItem::Final);

            else if (elem.attribute("type") == "initial") {
                // só podemos ter 1 estado inicial
                if (initialStates == 0)
                    state->setType(StateItem::Initial);
                else
                    qDebug() << filename << ": Mais de um estado inicial!";
                initialStates++;
            }

            else state->setType(StateItem::Normal);

            scene()->addItem(state);
            state->setX(elem.attribute("x").toDouble() + xoffset);
            state->setY(elem.attribute("y").toDouble() + yoffset);

            entry = entry.nextSibling();
        }
    }

    QDomNodeList transitionList = dom.elementsByTagName("state-transitions");
    for (int i = 0; i < transitionList.count(); i++) {
        QDomElement rootElement = transitionList.at(i).toElement();

        QDomNode entry = rootElement.firstChildElement("transition");
        while (!entry.isNull()) {
            QDomElement elem = entry.toElement();

            StateItem *from = stateMap[elem.attribute("from").toInt()];
            StateItem *to = stateMap[elem.attribute("to").toInt()];
            if (from == nullptr || to == nullptr) {
                qDebug() << elem.attribute("from") << "->" <<
                            elem.attribute("to") << ": Transição inválida!";
                entry = entry.nextSibling();
                continue;
            }

            TransitionItem *trans = new TransitionItem(this, from, to);

            trans->setToolTip(getComments(elem));

            QDomNode functions = elem.firstChildElement("function");
            QList<AcceptState> accepts;
            while (!functions.isNull()) {
                QDomElement func = functions.toElement();
                AcceptState ast;
                ast.accept = func.attribute("read");
                ast.write = func.attribute("write");
                if (func.attribute("move") == "left")
                    ast.direction = AcceptState::Left;
                else ast.direction = AcceptState::Right;

                accepts.push_back(ast);
                functions = functions.nextSibling();
            }

            trans->setAcceptList(accepts);
            scene()->addItem(trans);

            trans->setControlPoint(QPointF(elem.attribute("x").toDouble() + xoffset,
                                           elem.attribute("y").toDouble() + yoffset));

            entry = entry.nextSibling();
        }
    }

    qDebug() << "Arquivo carregado com sucesso!";
    return true;
}

void AutomataView::clear()
{
    scene()->clear();
    viewport()->update();
}

void AutomataView::setMachineComments(const QString &s)
{
    m_machineComment = s;
}

QString AutomataView::getMachineComments()
{
    return m_machineComment;
}

void AutomataView::play(int time)
{
    m_timer.start(time, this);
    emit playStatusChanged(true);
    lock(true);
}

void AutomataView::stop()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        emit playStatusChanged(false);
        lock(false);
    }
}

bool AutomataView::isPlaying()
{
    return m_timer.isActive();
}

int AutomataView::getStepCounter()
{
    return m_steps;
}

void AutomataView::resetMachine()
{
    if (m_tape != nullptr)
        m_tape->reset();
    m_steps = 0;
    emit stepCounterChanged(m_steps);

    MachineScene *mscene = dynamic_cast<MachineScene*>(scene());
    if (mscene == nullptr)
        return;

    mscene->setInativeAll();
    StateItem *initial = mscene->getInitialState();
    if (initial == nullptr) {
        QMessageBox::critical(this, "Erro", "Não existe um estado inicial!");
        return;
    }
    mscene->setActive(initial);
    viewport()->update();

}

void AutomataView::stepIn()
{
    if (m_tape == nullptr)
        return;

    MachineScene *mscene = dynamic_cast<MachineScene*>(scene());
    if (mscene == nullptr)
        return;

    StateItem *active = mscene->getActive();
    if (active == nullptr) {
        QMessageBox::critical(this, "Erro", "Não existe um estado ativo!");
        return;
    }

    if (active->getType() ==  StateItem::Final)
        return;

    if (active->getStatus() != StateItem::Jammed) {
        m_steps++;
        emit stepCounterChanged(m_steps);
    }

    QString symbol = m_tape->read();
    //qDebug() << "Leu na fita: " << symbol;
    TransitionAccept acc = active->next(symbol);
    if (acc.next != nullptr) {
        //qDebug() << "Aceitou transição para: " << acc.next->getNumber();
        mscene->setActive(acc.next);
        m_tape->write(acc.write);
        if (acc.direction == TransitionAccept::Left)
            m_tape->left();
        else if (acc.direction == TransitionAccept::Right)
            m_tape->right();
    } else {
        qDebug() << "Não existem transições!!!";
        mscene->setJam(active);
    }
}

void AutomataView::describe(QString comments)
{
    m_descriptor->setMachineComments(comments);
    m_descriptor->setMachineScene(static_cast<MachineScene*>(scene()));
    m_descriptor->exec();
}

void AutomataView::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        double scaleFactor = 1.15;
        if(event->delta() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    } else {
        QGraphicsView::wheelEvent(event);
    }

}

void AutomataView::timerEvent(QTimerEvent *event)
{
    event->accept();
    MachineScene *mscene = dynamic_cast<MachineScene*>(scene());
    if (mscene == nullptr) {
        stop();
        return;
    }

    StateItem *active = mscene->getActive();
    if (active == nullptr) {
        stop();
        return;
    }
    if (active->getType() == StateItem::Final ||
        active->getStatus() == StateItem::Jammed) {
        stop();
        return;
    }
    stepIn();
}

void AutomataView::lock(bool yes)
{
    MachineScene *mscene = dynamic_cast<MachineScene*>(scene());
    if (mscene == nullptr)
        return;

    mscene->setLock(yes);
}
