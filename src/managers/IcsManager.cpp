#include "managers/IcsManager.h"
#include "managers/EventManager.h"
#include "models/Event.h"

#include <QDate>
#include <QDateTime>
#include <QFile>
#include <QMap>
#include <QTextStream>
#include <QTimeZone>
#include <QDebug>

static QString escapeIcsText(const QString &text)
{
    QString result;
    result.reserve(text.size());

    for (const QChar ch : text)
    {
        if (ch == '\\')
            result += "\\\\";
        else if (ch == ',')
            result += "\\,";
        else if (ch == ';')
            result += "\\;";
        else if (ch == '\n' || ch == '\r')
            result += "\\n";
        else
            result += ch;
    }

    return result;
}

static QString unescapeIcsText(const QString &text)
{
    QString result;
    result.reserve(text.size());

    bool escaped = false;
    for (const QChar ch : text)
    {
        if (escaped)
        {
            if (ch == 'n' || ch == 'N')
                result += '\n';
            else
                result += ch;
            escaped = false;
            continue;
        }

        if (ch == '\\')
            escaped = true;
        else
            result += ch;
    }

    if (escaped)
        result += '\\';

    return result;
}

static QString formatIcsDateTime(const QDateTime &dateTime)
{
    return dateTime.toString("yyyyMMdd'T'HHmmss");
}

static QDateTime parseIcsDateTime(QString value)
{
    const int parameterIndex = value.indexOf(';');
    if (parameterIndex != -1)
        value = value.left(parameterIndex);

    if (value.endsWith('Z'))
    {
        QDateTime dateTime = QDateTime::fromString(value, "yyyyMMdd'T'HHmmss'Z'");
        if (dateTime.isValid())
            dateTime.setTimeZone(QTimeZone::utc());
        return dateTime.toLocalTime();
    }

    QDateTime dateTime = QDateTime::fromString(value, "yyyyMMdd'T'HHmmss");
    if (dateTime.isValid())
        return dateTime;

    const QDate date = QDate::fromString(value, "yyyyMMdd");
    if (date.isValid())
        return QDateTime(date, QTime(0, 0));

    return QDateTime();
}

static QString propertyName(const QString &line)
{
    const int colonIndex = line.indexOf(':');
    if (colonIndex == -1)
        return QString();

    const QString nameWithParameters = line.left(colonIndex);
    const int semicolonIndex = nameWithParameters.indexOf(';');
    return (semicolonIndex == -1 ? nameWithParameters : nameWithParameters.left(semicolonIndex)).toUpper();
}

static QString propertyValue(const QString &line)
{
    const int colonIndex = line.indexOf(':');
    if (colonIndex == -1)
        return QString();

    return line.mid(colonIndex + 1);
}

static QStringList unfoldIcsLines(const QString &content)
{
    QString normalized = content;
    normalized.replace("\r\n", "\n");
    normalized.replace('\r', '\n');

    QStringList unfolded;
    const QStringList lines = normalized.split('\n');
    for (const QString &line : lines)
    {
        if ((line.startsWith(' ') || line.startsWith('\t')) && !unfolded.isEmpty())
            unfolded.last() += line.mid(1);
        else
            unfolded.append(line);
    }

    return unfolded;
}

bool IcsManager::exportCalendarToIcs(int calendarId, const QString &filePath) const
{
    if (calendarId <= 0 || filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open ICS export file:" << file.errorString();
        return false;
    }

    EventManager eventManager;
    const std::vector<Event> events = eventManager.getEventsForCalendar(calendarId);

    QTextStream out(&file);
    out << "BEGIN:VCALENDAR\r\n";
    out << "VERSION:2.0\r\n";
    out << "PRODID:-//SyncCal//SyncCal Qt//EN\r\n";
    out << "CALSCALE:GREGORIAN\r\n";

    for (const Event &event : events)
    {
        out << "BEGIN:VEVENT\r\n";
        out << "UID:" << event.getId() << "@synccal\r\n";
        out << "SUMMARY:" << escapeIcsText(event.getTitle()) << "\r\n";
        out << "DESCRIPTION:" << escapeIcsText(event.getDescription()) << "\r\n";
        out << "LOCATION:" << escapeIcsText(event.getLocation()) << "\r\n";
        out << "DTSTART:" << formatIcsDateTime(event.getStartDateTime()) << "\r\n";
        out << "DTEND:" << formatIcsDateTime(event.getEndDateTime()) << "\r\n";
        out << "END:VEVENT\r\n";
    }

    out << "END:VCALENDAR\r\n";
    return true;
}

bool IcsManager::importEventsFromIcs(int calendarId, int userId, const QString &filePath)
{
    if (calendarId <= 0 || userId <= 0 || filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open ICS import file:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    const QStringList lines = unfoldIcsLines(in.readAll());

    EventManager eventManager;
    QMap<QString, QString> currentEvent;
    bool inEvent = false;
    bool importedAny = false;

    auto importCurrentEvent = [&]() {
        const QDateTime start = parseIcsDateTime(currentEvent.value("DTSTART"));
        const QDateTime end = parseIcsDateTime(currentEvent.value("DTEND"));
        if (!start.isValid() || !end.isValid() || start >= end)
            return;

        Event event(
            -1,
            calendarId,
            unescapeIcsText(currentEvent.value("SUMMARY")),
            unescapeIcsText(currentEvent.value("DESCRIPTION")),
            unescapeIcsText(currentEvent.value("LOCATION")),
            start,
            end,
            userId
        );

        if (eventManager.createEvent(event))
            importedAny = true;
    };

    for (const QString &line : lines)
    {
        if (line == "BEGIN:VEVENT")
        {
            currentEvent.clear();
            inEvent = true;
            continue;
        }

        if (line == "END:VEVENT")
        {
            if (inEvent)
                importCurrentEvent();
            currentEvent.clear();
            inEvent = false;
            continue;
        }

        if (!inEvent)
            continue;

        const QString name = propertyName(line);
        if (name == "SUMMARY" || name == "DESCRIPTION" || name == "LOCATION" || name == "DTSTART" || name == "DTEND")
            currentEvent[name] = propertyValue(line);
    }

    return importedAny;
}
