#include <gtest/gtest.h>
#include "managers/DatabaseManager.h"
#include "managers/EventManager.h"
#include "models/Event.h"
#include <QDateTime>
#include <QSqlQuery>

class EventManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        DatabaseManager::instance().openDatabase(":memory:");

        QSqlDatabase db = DatabaseManager::instance().getDatabase();
        QSqlQuery query(db);
        query.exec("DELETE FROM calendar_invitations");
        query.exec("DELETE FROM events");
        query.exec("DELETE FROM calendar_members");
        query.exec("DELETE FROM calendars");
        query.exec("DELETE FROM users");

        query.exec("INSERT INTO users (id, username, email, password) "
                   "VALUES (1, 'owner', 'owner@example.com', 'password')");
        query.exec("INSERT INTO calendars (id, name, owner_id) "
                   "VALUES (1, 'Test Calendar', 1)");
    }
};

// Basic EventManager tests (may require DB backend)
TEST_F(EventManagerTest, CreateEventAndConflict) {
    EventManager em;
    QDateTime start = QDateTime::currentDateTime();
    QDateTime end = start.addSecs(3600);
    Event e(0, 1, "Test Event", "Desc", "Loc", start, end, 1);

    EXPECT_TRUE(em.createEvent(e));
    EXPECT_TRUE(em.hasConflict(1, start, end));
}
