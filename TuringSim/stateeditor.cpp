#include <QtWidgets>
#include <QStringList>
#include "stateeditor.h"

StateEditor::StateEditor(QWidget *parent) :
    QDialog(parent)
{
    m_number = new QSpinBox();
    m_type = new QComboBox();
    m_ok = new QPushButton("Ok");
    m_cancel = new QPushButton("Cancelar");
    m_edit = new QPlainTextEdit;

    m_type->addItems(QStringList() << "Normal" << "Inicial" << "Final");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_number);
    layout->addWidget(m_type);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(m_cancel);
    buttons->addWidget(m_ok);
    layout->addWidget(m_edit);
    layout->addLayout(buttons);
    setLayout(layout);

    //adjustSize();
    //setFixedSize(size());

    connect(m_ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle("Editar estado");
}

void StateEditor::setState(const StateItem *item)
{
    m_edit->setPlainText(item->toolTip());
    if (item != nullptr) {
        m_number->setValue(item->getNumber());
        switch (item->getType()) {
        case StateItem::Initial:
            m_type->setCurrentText(tr("Inicial"));
            break;
        case StateItem::Final:
            m_type->setCurrentText(tr("Final"));
            break;
        case StateItem::Normal:
            m_type->setCurrentText(tr("Normal"));
            break;
        }
    }
}

int StateEditor::getNumber() const
{
    return m_number->value();
}

StateItem::StateType StateEditor::getType() const
{
    switch (m_type->currentIndex()) {
    case 0: return StateItem::Normal;
    case 1: return StateItem::Initial;
    case 2: return StateItem::Final;
    default:
        return StateItem::Normal;
    }
}

QString StateEditor::getComment()
{
    return m_edit->toPlainText();
}
