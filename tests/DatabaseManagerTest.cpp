#include <gtest/gtest.h>
#include "managers/DatabaseManager.h"

// Simple tests for DatabaseManager singleton
TEST(DatabaseManagerTest, InstanceOpenCreateTables) {
    DatabaseManager &db = DatabaseManager::instance();
    EXPECT_TRUE(db.openDatabase(":memory:"));
    EXPECT_TRUE(db.createTables());
    EXPECT_TRUE(db.getDatabase().isValid());
}
