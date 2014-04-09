#ifndef TAPEVIEWER_H
#define TAPEVIEWER_H

#include <QWidget>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QVector>
#include <QBasicTimer>
#include <QSize>

#include "tape.h"

class TapeView : public QWidget
{
    Q_OBJECT
public:
    explicit TapeView(QWidget *parent = 0);

    QSize sizeHint() const;
public slots:
    void seekTo(int pos);
    void setSpeed(int factor);
    QString read();
    void write(QString symbol);
    void writeString(QString symbol);
    void clear();
    void left();
    void right();
    void reset();
signals:
    void valueChanged(int);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void timerEvent(QTimerEvent *event);
private:
    QFont m_typeWriterFont;
    void drawWidget(QPainter &p);
    QVector<QChar> m_inputList;
    int m_headerPos;
    int m_seekToPos;
    int m_displace;
    int m_speed;
    QBasicTimer m_timer;
    int m_seekAccel;
    bool m_isSeeking;
    QSize m_symSize;
    Tape m_tape;
public:
    int getTapeSize() const { return m_inputList.size(); }
};

#endif // TAPEVIEWER_H
