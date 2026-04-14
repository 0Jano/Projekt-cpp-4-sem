#include <gtest/gtest.h>
#include "managers/AuthManager.h"
#include "models/User.h"

// Test fixture for AuthManager
class AuthManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }

    AuthManager authManager;
};

// Test registering a new user
TEST_F(AuthManagerTest, RegisterNewUser) {
    User user("testuser", "test@example.com", "password123");
    EXPECT_TRUE(authManager.registerUser(user));
}

// Test registering a user with existing email
TEST_F(AuthManagerTest, RegisterUserWithExistingEmail) {
    User user1("user1", "same@example.com", "pass1");
    User user2("user2", "same@example.com", "pass2");

    EXPECT_TRUE(authManager.registerUser(user1));
    EXPECT_FALSE(authManager.registerUser(user2));
}

// Test successful login with email
TEST_F(AuthManagerTest, LoginSuccessfulEmail) {
    User user("testuser", "test@example.com", "password123");
    authManager.registerUser(user);

    EXPECT_TRUE(authManager.loginUser("test@example.com", "password123"));
}

// Test successful login with username
TEST_F(AuthManagerTest, LoginSuccessfulUsername) {
    User user("testuser", "test@example.com", "password123");
    authManager.registerUser(user);

    EXPECT_TRUE(authManager.loginUser("testuser", "password123"));
}

// Test login with wrong password
TEST_F(AuthManagerTest, LoginWrongPassword) {
    User user("testuser", "test@example.com", "password123");
    authManager.registerUser(user);

    EXPECT_FALSE(authManager.loginUser("test@example.com", "wrongpassword"));
}

// Test login with non-existent email/username
TEST_F(AuthManagerTest, LoginNonExistent) {
    EXPECT_FALSE(authManager.loginUser("nonexistent@example.com", "password123"));
    EXPECT_FALSE(authManager.loginUser("nonexistentuser", "password123"));
}

// Test login with empty credentials
TEST_F(AuthManagerTest, LoginEmptyCredentials) {
    EXPECT_FALSE(authManager.loginUser("", ""));
}