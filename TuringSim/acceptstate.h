#ifndef ACCEPTSTATE_H
#define ACCEPTSTATE_H

#include <QString>

struct AcceptState {
    QString accept;
    QString write;
    int direction;
    enum {
        Left = 0,
        Right
    };
    QString toString() {
        QString t(accept);
        t += ",";
        t += write;
        t += ",";
        if (direction)
            t += "D";
        else t += "E";
        return t;
    }
    QString dirToString() {
        if (direction == Left)
            return "left";
        else return "right";
    }
};

Q_DECLARE_METATYPE(AcceptState);

#endif // ACCEPTSTATE_H
