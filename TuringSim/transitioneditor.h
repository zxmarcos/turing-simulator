#ifndef TRANSITIONEDITOR_H
#define TRANSITIONEDITOR_H

#include <QDialog>
#include <QtWidgets>
#include <QList>
#include "transitionitem.h"
class TransitionEditor : public QDialog
{
    Q_OBJECT
public:
    explicit TransitionEditor(QWidget *parent = 0);

    void setAcceptList(const QList<AcceptState> &list);
    QList<AcceptState> getAcceptList();
    void setComment(QString str);
    QString getComment();
signals:
public slots:

    void insertNew();
    void cancel();
    void save();
    void setCurrentAcceptState(int row);
    void edit();
    void remove();
private:
    enum {
        Insert = 0,
        Edit
    };
    int m_currentPos;
    int m_operation;
    void enableControls(bool enable);
    bool m_editing;
    void clear();
    void clearControls();
    QLineEdit *m_accept;
    QLineEdit *m_write;
    QButtonGroup *m_directionGroup;
    QToolButton *m_rightDirection;
    QToolButton *m_leftDirection;
    QPushButton *m_dialogOk;
    QPushButton *m_dialogCancel;
    QListWidget *m_list;
    QPushButton *m_addButton;
    QPushButton *m_cancelButton;
    QPushButton *m_editButton;
    QPushButton *m_saveButton;
    QPushButton *m_removeButton;
    QPlainTextEdit *m_edit;
};

#endif // TRANSITIONEDITOR_H
