#include <iostream>
#include <cassert>
#include <sstream>

// Mocks and utilities
class DatabaseMock {
public:
    bool doesUserExist(const std::string& username) {
        // Simulate database check
        return username == "admin";
    }

    void initializeAdminDatabase(const std::string& privateKey) {
        // Simulate database initialization for admin
        std::cout << "Admin database initialized with private key: " << privateKey << std::endl;
    }
};

class UserManagerMock {
public:
    UserManagerMock(DatabaseMock* db) {}

    void createUser(const std::string& username) {
        // Simulate user creation
        std::cout << "User " << username << " created." << std::endl;
    }
};

// Redirect std::cin and std::cout
class TestIO {
    std::streambuf* cinBackup;
    std::streambuf* coutBackup;
    std::istringstream cinStream;
    std::ostringstream coutStream;

public:
    TestIO(const std::string& input) : cinStream(input) {
        cinBackup = std::cin.rdbuf(cinStream.rdbuf());
        coutBackup = std::cout.rdbuf(coutStream.rdbuf());
    }

    ~TestIO() {
        std::cin.rdbuf(cinBackup);
        std::cout.rdbuf(coutBackup);
    }

    std::string getOutput() {
        return coutStream.str();
    }
};

// CipherFS class for testing
class CipherFSTest {
public:
    static void runTests() {
        testFirstRunCreation();
        testKeyfilePrompt();
        testUserLogin();
        // Add more tests as needed
    }

private:
    static void testFirstRunCreation() {
        TestIO testIO(""); // No input needed for this test
        char* argv[] = {strdup("./fileserver"), nullptr};
        CipherFS(1, argv);
        assert(testIO.getOutput().find("Created admin!") != std::string::npos);
        std::cout << "testFirstRunCreation: Passed" << std::endl;
    }

    static void testKeyfilePrompt() {
        TestIO testIO(""); // No input needed for this test
        char* argv[] = {strdup("./fileserver"), nullptr};
        CipherFS(1, argv);
        assert(testIO.getOutput().find("Please enter the keyfile name!") != std::string::npos);
        std::cout << "testKeyfilePrompt: Passed" << std::endl;
    }

    static void testUserLogin() {
        TestIO testIO(""); // Simulate keyfile name input
        char* argv[] = {strdup("./fileserver"), strdup("keyfile.txt")};
        CipherFS(2, argv);
        // Assuming loginUser and CLI are implemented correctly, check for expected output
        assert(testIO.getOutput().find("Welcome back,") != std::string::npos);
        std::cout << "testUserLogin: Passed" << std::endl;
    }

    // Implement loginUser and CLI class stubs or mocks if necessary
};

int main() {
    CipherFSTest::runTests();
    return 0;
}
