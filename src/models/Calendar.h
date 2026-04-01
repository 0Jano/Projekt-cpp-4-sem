#ifndef CALENDAR_H
#define CALENDAR_H

#include <QString>

class Calendar
{
public:
    Calendar();
    Calendar(int id, const QString &name, int ownerId, const QString &type);

    int getId() const;
    QString getName() const;
    int getOwnerId() const;
    QString getType() const;

private:
    int id;
    QString name;
    int ownerId;
    QString type;
};

#endif