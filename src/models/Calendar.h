#ifndef CALENDAR_H
#define CALENDAR_H

#include <QString>

class Calendar
{
public:
    Calendar();
    Calendar(int id, const QString &name, int ownerId);

    int getId() const;
    QString getName() const;
    int getOwnerId() const;

private:
    int id;
    QString name;
    int ownerId;
};

#endif
