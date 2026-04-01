#include "models/Calendar.h"

Calendar::Calendar() : id(-1), ownerId(-1) {}

Calendar::Calendar(int id, const QString &name, int ownerId, const QString &type)
    : id(id), name(name), ownerId(ownerId), type(type) {}

int Calendar::getId() const
{
    return id;
}

QString Calendar::getName() const
{
    return name;
}

int Calendar::getOwnerId() const
{
    return ownerId;
}

QString Calendar::getType() const
{
    return type;
}