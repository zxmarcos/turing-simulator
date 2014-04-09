#include <QtWidgets>
#include "transitioneditor.h"

TransitionEditor::TransitionEditor(QWidget *parent) :
    QDialog(parent)
{
    m_accept = new QLineEdit;
    m_write = new QLineEdit;
    m_dialogOk = new QPushButton("Ok");
    m_dialogCancel = new QPushButton("Cancelar");
    m_list = new QListWidget;
    m_edit = new QPlainTextEdit;

    m_leftDirection = new QToolButton;
    m_leftDirection->setText("E");
    m_rightDirection = new QToolButton;
    m_rightDirection->setText("D");

    m_directionGroup = new QButtonGroup;
    m_directionGroup->setExclusive(true);
    m_directionGroup->addButton(m_leftDirection, AcceptState::Left);
    m_directionGroup->addButton(m_rightDirection, AcceptState::Right);

    m_leftDirection->setAutoRaise(true);
    m_leftDirection->setCheckable(true);
    m_rightDirection->setAutoRaise(true);
    m_rightDirection->setCheckable(true);

    m_addButton = new QPushButton("Novo");
    m_cancelButton = new QPushButton("Cancelar");
    m_editButton = new QPushButton("Editar");
    m_saveButton = new QPushButton("Salvar");
    m_removeButton = new QPushButton("Remover");

    QHBoxLayout *editButtons = new QHBoxLayout;
    editButtons->addWidget(m_addButton);
    editButtons->addWidget(m_editButton);
    editButtons->addWidget(m_removeButton);
    editButtons->addWidget(m_cancelButton);
    editButtons->addWidget(m_saveButton);

    QHBoxLayout *dlgbuttons = new QHBoxLayout;
    dlgbuttons->addStretch();
    dlgbuttons->addWidget(m_dialogCancel);
    dlgbuttons->addWidget(m_dialogOk);

    connect(m_dialogOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_dialogCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *main = new QVBoxLayout;
    QHBoxLayout *dlgedit = new QHBoxLayout;

    QLabel *labelAccept = new QLabel("Aceita:");
    QLabel *labelWrite = new QLabel("Escreve:");
    QLabel *labelDirection = new QLabel("Direção:");

    QVBoxLayout *layAccept = new QVBoxLayout;
    layAccept->addWidget(labelAccept);
    layAccept->addWidget(m_accept);

    QVBoxLayout *layWrite = new QVBoxLayout;
    layWrite->addWidget(labelWrite);
    layWrite->addWidget(m_write);

    dlgedit->addLayout(layAccept);
    dlgedit->addLayout(layWrite);

    QHBoxLayout *dirGroupLayout = new QHBoxLayout;
    dirGroupLayout->setSpacing(0);
    dirGroupLayout->addWidget(m_leftDirection);
    dirGroupLayout->addWidget(m_rightDirection);

    QVBoxLayout *layoutDirection = new QVBoxLayout;
    layoutDirection->addWidget(labelDirection);
    layoutDirection->addLayout(dirGroupLayout);

    dlgedit->addLayout(layoutDirection);
    main->addLayout(dlgedit);
    main->addLayout(editButtons);
    main->addWidget(m_list);
    main->addWidget(m_edit);
    main->addLayout(dlgbuttons);

    setWindowTitle(tr("Editor de transições"));
    setLayout(main);
    enableControls(false);

    m_operation = -1;
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(insertNew()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(m_list, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentAcceptState(int)));
    connect(m_editButton, SIGNAL(clicked()), this, SLOT(edit()));
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(remove()));

    QFont tw("Monospaced");
    tw.setStyleHint(QFont::TypeWriter);
    tw.setPointSize(12);
    m_list->setFont(tw);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_currentPos = -1;


}

void TransitionEditor::setAcceptList(const QList<AcceptState> &list)
{
    clear();
    clearControls();
    enableControls(false);
    m_currentPos = -1;
    foreach (AcceptState as, list) {
        QListWidgetItem *item = new QListWidgetItem(as.toString());
        item->setData(Qt::UserRole, QVariant::fromValue(as));
        m_list->addItem(item);
    }
    m_list->clearSelection();
    //m_list->currentItem()->data(Qt::UserRole).value<AcceptState>();
}

QList<AcceptState> TransitionEditor::getAcceptList()
{
    QList<AcceptState> list;
    for (int i = 0; i < m_list->count(); i++) {
        list.push_back(m_list->item(i)->data(Qt::UserRole).value<AcceptState>());
    }
    return list;
}

void TransitionEditor::setComment(QString str)
{
    m_edit->setPlainText(str);
}

QString TransitionEditor::getComment()
{
    return m_edit->toPlainText();
}

void TransitionEditor::insertNew()
{
    m_operation = Insert;
    enableControls(true);
    m_leftDirection->setChecked(true);
    m_list->setEnabled(false);
}

void TransitionEditor::cancel()
{
    enableControls(false);
    if (m_operation != Edit) {
        clearControls();
        m_currentPos = -1;
    } else {
        setCurrentAcceptState(m_currentPos);
    }
}

void TransitionEditor::save()
{
    AcceptState as;
    as.accept = m_accept->text().simplified();
    as.write = m_write->text().simplified();
    as.direction = m_directionGroup->checkedId();
    if (m_operation == Insert) {
        QListWidgetItem *item = new QListWidgetItem(as.toString());
        item->setData(Qt::UserRole, QVariant::fromValue(as));
        m_list->addItem(item);
        clearControls();
        enableControls(false);
        m_currentPos = -1;
    } else if (m_operation == Edit) {
        QListWidgetItem *item = m_list->currentItem();
        item->setText(as.toString());
        item->setData(Qt::UserRole, QVariant::fromValue(as));
        enableControls(false);
    }
    m_operation = -1;
    m_list->clearSelection();
}

void TransitionEditor::setCurrentAcceptState(int row)
{
    QListWidgetItem *item = m_list->item(row);
    if (item) {
        m_currentPos = row;
        AcceptState as = item->data(Qt::UserRole).value<AcceptState>();
        m_accept->setText(as.accept);
        m_write->setText(as.write);
        if (as.direction == AcceptState::Left)
            m_leftDirection->setChecked(true);
        else m_rightDirection->setChecked(true);
    } else clearControls();
}

void TransitionEditor::edit()
{
    if (m_currentPos == -1)
        return;
    m_operation = Edit;
    enableControls(true);
    m_list->setEnabled(false);
}

void TransitionEditor::remove()
{
    QListWidgetItem *item = m_list->takeItem(m_currentPos);
    if (item) {
        delete item;
    }
}

void TransitionEditor::enableControls(bool enable)
{
    m_accept->setEnabled(enable);
    m_write->setEnabled(enable);
    m_leftDirection->setEnabled(enable);
    m_rightDirection->setEnabled(enable);
    m_saveButton->setEnabled(enable);
    m_cancelButton->setEnabled(enable);
    m_addButton->setEnabled(!enable);
    m_editButton->setEnabled(!enable);
    m_removeButton->setEnabled(!enable);
    m_list->setEnabled(!enable);
}

void TransitionEditor::clearControls()
{
    m_accept->clear();
    m_write->clear();
    m_leftDirection->setChecked(false);
    m_rightDirection->setChecked(false);
}

void TransitionEditor::clear()
{
    while (m_list->count() > 0) {
        QListWidgetItem *item = m_list->takeItem(0);
        delete item;
    }
}

