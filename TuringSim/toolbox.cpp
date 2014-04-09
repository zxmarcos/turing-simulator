#include <QtWidgets>
#include "toolbox.h"
#include "machinescene.h"

Toolbox::Toolbox(QWidget *parent) :
    QFrame(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    QButtonGroup *group = new QButtonGroup(this);
    QVBoxLayout *layout = new QVBoxLayout;
    group->setExclusive(true);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    m_select = new QToolButton();
    m_select->setCheckable(true);
    m_select->setAutoRaise(true);
    m_select->setIcon(QIcon(tr(":/images/cursor_arrow.png")));
    m_select->setToolTip("Mover ou editar propriedades dos items");
    group->addButton(m_select, MachineScene::Select);
    layout->addWidget(m_select);

    m_insertState = new QToolButton();
    m_insertState->setCheckable(true);
    m_insertState->setAutoRaise(true);
    m_insertState->setIcon(QIcon(tr(":/images/new-state.png")));
    m_insertState->setToolTip("Insere um novo estado");
    group->addButton(m_insertState, MachineScene::InsertState);
    layout->addWidget(m_insertState);

    m_insertTransition = new QToolButton();
    m_insertTransition->setCheckable(true);
    m_insertTransition->setAutoRaise(true);
    m_insertTransition->setIcon(QIcon(tr(":/images/new-transition.png")));
    m_insertTransition->setToolTip("Cria uma nova transição");
    group->addButton(m_insertTransition, MachineScene::InsertTransition);
    layout->addWidget(m_insertTransition);

    m_deleteItem = new QToolButton();
    m_deleteItem->setCheckable(true);
    m_deleteItem->setAutoRaise(true);
    m_deleteItem->setIcon(QIcon(tr(":/images/delete-item.png")));
    m_deleteItem->setToolTip("Remove items da máquina");
    group->addButton(m_deleteItem, MachineScene::DeleteItem);
    layout->addWidget(m_deleteItem);

    layout->setContentsMargins(4, 3, 0, 0);
    setLayout(layout);
    adjustSize();
    setFixedWidth(width());
    setAutoFillBackground(true);

    connect(group, SIGNAL(buttonPressed(int)), this, SIGNAL(toolChanged(int)));
}
