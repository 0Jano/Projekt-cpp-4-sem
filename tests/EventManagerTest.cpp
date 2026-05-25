#include <gtest/gtest.h>
#include "managers/EventManager.h"
#include "models/Event.h"
#include <QDateTime>

// Basic EventManager tests (may require DB backend)
TEST(EventManagerTest, CreateEventAndConflict) {
    EventManager em;
    QDateTime start = QDateTime::currentDateTime();
    QDateTime end = start.addSecs(3600);
    Event e(0, 1, "Test Event", "Desc", "Loc", start, end, 1);

    EXPECT_TRUE(em.createEvent(e));
    EXPECT_TRUE(em.hasConflict(1, start, end));
}
