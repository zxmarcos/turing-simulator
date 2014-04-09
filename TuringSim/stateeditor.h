#ifndef STATEEDITOR_H
#define STATEEDITOR_H

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include "stateitem.h"

class StateEditor : public QDialog
{
    Q_OBJECT
public:
    explicit StateEditor(QWidget *parent = 0);

    void setState(const StateItem *item);

    int getNumber() const;
    StateItem::StateType getType() const;
    QString getComment();
signals:
public slots:

private:
    QSpinBox *m_number;
    QComboBox *m_type;
    QPushButton *m_ok;
    QPushButton *m_cancel;
    QPlainTextEdit *m_edit;
};

#endif // STATEEDITOR_H
