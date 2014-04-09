#ifndef TAPE_H
#define TAPE_H

#include <QMap>
#include <QString>

class Tape
{
public:
    Tape();

    void setString(int position, QString str);
    void clear();
    int size() const;
    QString read(int position);
    void write(int position, QString value);
private:
    QMap<int, QString> m_memory;
};

#endif // TAPE_H
