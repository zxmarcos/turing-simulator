#include <QString>
#include <QMap>
#include "tape.h"

Tape::Tape()
{
}

QString Tape::read(int position)
{
    if (m_memory.find(position) == m_memory.end())
        write(position, "#");
    return m_memory[position];
}

void Tape::write(int position, QString value)
{
    m_memory[position] = value;
}

void Tape::setString(int position, QString str)
{
    foreach (QChar s, str) {
        write(position++, QString(s));
    }
}

void Tape::clear()
{
    m_memory.clear();
}

int Tape::size() const
{
    return m_memory.size();
}
