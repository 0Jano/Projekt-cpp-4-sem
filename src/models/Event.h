#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QDateTime>

/**
 * @brief Represents a single calendar event with a time range.
 *
 * @details Stores event metadata, location, start and end datetimes, and the
 * user who created the event. The model is used by EventManager for database
 * create, update, and query operations.
 */
class Event
{
public:
    /**
     * @brief Constructs an empty event model.
     */
    Event();

    /**
     * @brief Constructs an event model with all persisted fields.
     */
    Event(int id,
          int calendarId,
          const QString &title,
          const QString &description,
          const QString &location,
          const QDateTime &startDateTime,
          const QDateTime &endDateTime,
          int createdBy);

    /**
     * @brief Returns the event ID.
     * @return Database identifier of the event.
     */
    int getId() const;

    /**
     * @brief Returns the calendar ID.
     * @return ID of the calendar containing the event.
     */
    int getCalendarId() const;

    /**
     * @brief Returns the event title.
     * @return Title stored for the event.
     */
    QString getTitle() const;

    /**
     * @brief Returns the event description.
     * @return Description stored for the event.
     */
    QString getDescription() const;

    /**
     * @brief Returns the event location.
     * @return Location stored for the event.
     */
    QString getLocation() const;

    /**
     * @brief Returns the event start date and time.
     * @return Start date and time of the event.
     */
    QDateTime getStartDateTime() const;

    /**
     * @brief Returns the event end date and time.
     * @return End date and time of the event.
     */
    QDateTime getEndDateTime() const;

    /**
     * @brief Returns the creator user ID.
     * @return User ID of the event creator.
     */
    int getCreatedBy() const;

private:
    int id; ///< Database identifier of the event.
    int calendarId; ///< ID of the calendar containing the event.
    QString title; ///< Event title.
    QString description; ///< Event description.
    QString location; ///< Event location.
    QDateTime startDateTime; ///< Start date and time of the event.
    QDateTime endDateTime; ///< End date and time of the event.
    int createdBy; ///< User ID of the event creator.
};

#endif
