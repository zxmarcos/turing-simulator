#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QFrame>
#include <QToolBar>
#include <QToolButton>

class Toolbox : public QFrame
{
    Q_OBJECT
public:
    explicit Toolbox(QWidget *parent = 0);

signals:
    void toolChanged(int);
private:
    QToolButton *m_select;
    QToolButton *m_insertState;
    QToolButton *m_insertTransition;
    QToolButton *m_deleteItem;
};

#endif // TOOLBOX_H
