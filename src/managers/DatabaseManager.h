#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

/**
 * @brief Singleton that owns the QSqlDatabase connection and creates tables.
 *
 * @details Opens the application SQLite database, enables required database
 * settings, and initializes tables and indexes on first run. Manager classes
 * retrieve the shared database handle from this singleton.
 */
class DatabaseManager
{
public:
    /**
     * @brief Returns the singleton database manager instance.
     * @return Shared DatabaseManager instance.
     */
    static DatabaseManager& instance();

    /**
     * @brief Opens the SQLite database and initializes schema objects.
     * @param databaseName Name or path of the SQLite database file.
     * @return true if the database opened and table creation succeeded.
     */
    bool openDatabase(const QString &databaseName = "synccal.db");

    /**
     * @brief Creates all tables and indexes if they do not exist.
     * @return true if every schema statement succeeded.
     */
    bool createTables();

    /**
     * @brief Returns the underlying Qt SQL database handle.
     * @return QSqlDatabase handle used by manager classes.
     */
    QSqlDatabase getDatabase() const;

private:
    DatabaseManager();
    QSqlDatabase db; ///< Shared Qt SQL database connection handle.
};

#endif
