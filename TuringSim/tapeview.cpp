#include <QtWidgets>
#include "tapeview.h"
#include "tape.h"

TapeView::TapeView(QWidget *parent) :
    QWidget(parent)
{
    m_typeWriterFont = QFont("Monospaced");
    m_typeWriterFont.setStyleHint(QFont::TypeWriter);
    m_typeWriterFont.setPointSize(11);

    m_tape.setString(0, "@");
    m_headerPos = 0;
    m_displace = 0;
    m_seekToPos = 0;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_seekAccel = 0;
    m_isSeeking = false;
    m_speed = 5;
}

void TapeView::seekTo(int pos)
{
    if (m_headerPos == pos && m_seekToPos == pos)
        return;
    emit valueChanged(pos);
    m_seekToPos = pos;
#if 1
    m_isSeeking = true;
    m_seekAccel = abs(m_headerPos - pos) * m_speed;
    if (!m_timer.isActive())
        m_timer.start(10, this);
#else
    m_headerPos = pos;
#endif
    update();
}

void TapeView::setSpeed(int factor)
{
    m_speed = factor;
}

QString TapeView::read()
{
    return m_tape.read(m_seekToPos);
}

void TapeView::write(QString symbol)
{
    m_tape.write(m_seekToPos, symbol);
    update();
}

void TapeView::writeString(QString symbol)
{
    m_tape.setString(m_seekToPos, symbol);
    update();
}

void TapeView::clear()
{
    m_tape.clear();
    update();
}

void TapeView::left()
{
    seekTo(m_seekToPos - 1);
}

void TapeView::right()
{
    seekTo(m_seekToPos + 1);
}

void TapeView::reset()
{
    m_headerPos = 0;
    m_seekToPos = 0;
    seekTo(0);
}

QSize TapeView::sizeHint() const
{
    return QSize(300, 31);
}

void TapeView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter qp(this);
    qp.setClipRect(event->rect());
    drawWidget(qp);
}

void TapeView::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if (!m_isSeeking) {
        m_timer.stop();
        return;
    }

    if (m_seekToPos < m_headerPos) {
        m_displace += m_seekAccel;
        if (abs(m_displace) >= m_symSize.width()) {
            m_headerPos--;
            m_displace = 0;
        }
    } else {
        m_displace -= m_seekAccel;
        if (abs(m_displace) >= m_symSize.width()) {
            m_headerPos++;
            m_displace = 0;
        }
    }

    if (m_headerPos == m_seekToPos) {
        m_timer.stop();
        m_isSeeking = false;
    }

    update();
}

void TapeView::drawWidget(QPainter &p)
{
    // Cálculos de posições altamente baseados em Extreme Go Horse kkkkk

    p.setFont(m_typeWriterFont);
    QFontMetrics metrics(m_typeWriterFont);
    const int symRectPaddingX = 15;
    const int symRectPaddingY = 10;
    const QPoint center(width() / 2, height() / 2);
    m_symSize = QSize(metrics.width(' ') + symRectPaddingX,
                     metrics.height() + symRectPaddingY);
    const QColor tapeColor(240, 240, 200);
    const QColor tapeColorShadow(0x80, 0x80, 0x80);
    const int charWidth = metrics.width(' ');
    const int charHeight = metrics.height();


    // desenha a fita
    p.fillRect(QRect(0,
                     center.y() - m_symSize.height() / 2,
                     width(),
                     m_symSize.height()),
               tapeColor);
    p.setPen(tapeColorShadow);
    p.drawRect(QRect(0,
                     center.y() - m_symSize.height() / 2,
                     width(),
                     m_symSize.height()).adjusted(0, 0, -1, 0));

    // desenha os quadrados (posições)
    p.setPen(tapeColorShadow);

    QPen black(Qt::black);
    int tapePosition = m_headerPos;
    bool firstPainted = false;
    for (int x = m_displace + center.x() + m_symSize.width() / 2; x > 0; ) {
        if (tapePosition == 0) {
            p.fillRect(QRect(x - m_symSize.width(),
                             center.y() - m_symSize.height() / 2,
                             m_symSize.width(),
                             m_symSize.height()).adjusted(1, 1, 0, 0),
                       QColor(100,255,100));
            firstPainted = true;
        }
        p.setPen(black);
        p.drawText((x - m_symSize.width() / 2) - charWidth / 2,
                   center.y() + charHeight / 3,
                   QString(m_tape.read(tapePosition)));

        tapePosition--;
        x -= m_symSize.width();
        p.setPen(tapeColorShadow);
        p.drawLine(x, center.y() - m_symSize.height() / 2,
                   x, center.y() + m_symSize.height() / 2);

    }
    tapePosition = m_headerPos;
    for (int x = m_displace + center.x() - m_symSize.width() / 2; x < width(); ) {
        if (tapePosition == 0 && !firstPainted) {
            p.fillRect(QRect(x,
                             center.y() - m_symSize.height() / 2,
                             m_symSize.width(),
                             m_symSize.height()).adjusted(1, 1, 1, 0),
                       QColor(100,255,100));
        }

        p.setPen(tapeColorShadow);
        p.drawLine(x, center.y() - m_symSize.height() / 2,
                   x, center.y() + m_symSize.height() / 2);

        p.setPen(black);
        p.drawText((x + m_symSize.width() / 2) - charWidth / 2,
                   center.y() + charHeight / 3,
                   QString(m_tape.read(tapePosition)));
        x += m_symSize.width();
        tapePosition++;
    }


#if 1
    // Desenha as sombras
    const int shadowSize = width() / 4;
    const int initAlpha = 60;

    QRect left(0, center.y() - m_symSize.height() / 2,
                  shadowSize,
                  m_symSize.height());

    QLinearGradient gradient1(left.topLeft(), left.topRight());
    gradient1.setColorAt(0, QColor(35, 35, 10, initAlpha));
    gradient1.setColorAt(1, QColor(35, 35, 10, 0));
    p.fillRect(left, gradient1);

    QRect right(width() - shadowSize, center.y() - m_symSize.height() / 2,
                  shadowSize,
                  m_symSize.height());

    QLinearGradient gradient2(right.topRight(), right.topLeft());
    gradient2.setColorAt(0, QColor(35, 35, 10, initAlpha));
    gradient2.setColorAt(1, QColor(35, 35, 10, 0));
    p.fillRect(right, gradient2);
#endif

    // desenha o cabeçote, sempre ao centro
    p.setPen(Qt::red);
    p.fillRect(QRect(center.x() - m_symSize.width() / 2,
                     center.y() - m_symSize.height() / 2,
                     m_symSize.width(), m_symSize.height()),
               QColor(255, 0, 0, 40));
    p.drawRect(QRect(center.x() - m_symSize.width() / 2,
                     center.y() - m_symSize.height() / 2,
                     m_symSize.width(), m_symSize.height()));
}
