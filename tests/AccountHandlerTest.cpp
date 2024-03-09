#include "AccountHandler.h"
#include <iostream>
#include <cassert>
#include <string>

// Mock implementations or stubs for Database and FileSystemService need to be included or defined here
#include "MockDatabase.h"
#include "MockFileSystemService.h"

class AccountHandlerTest {
public:
    void runTests() {
        testCreateUserNonExisting();
        testCreateUserExisting();
    }

private:
    MockDatabase db;
    MockFileSystemService filesystemService;

    void setup() {
        // Setup your mock database and filesystem service with expected behaviors
        db.reset();
        filesystemService.reset();
    }

    void testCreateUserNonExisting() {
        setup(); // Reset mocks to a known state

        CipherFS::AccountHandler accountHandler(&db);
        accountHandler.setFileSystemService(&filesystemService);

        std::string userName = "newuser";
        std::string expectedHomeDir = "/newuser/";

        // Setup expectations on the mock objects
        db.expectUserExistence(userName, false); // Expect that the user does not exist
        filesystemService.expectDirectoryCreation(expectedHomeDir);
        filesystemService.expectDirectoryCreation(expectedHomeDir + "personal/");
        filesystemService.expectDirectoryCreation(expectedHomeDir + "shared/");

        std::string homeDir = accountHandler.createUser(userName);

        assert(homeDir == expectedHomeDir);
        std::cout << "testCreateUserNonExisting: Passed" << std::endl;
    }

    void testCreateUserExisting() {
        setup(); // Reset mocks to a known state

        CipherFS::AccountHandler accountHandler(&db);
        accountHandler.setFileSystemService(&filesystemService);

        std::string userName = "existinguser";
        std::string expectedHomeDir = "/existinguser/";

        // Setup expectations on the mock objects
        db.expectUserExistence(userName, true); // Expect that the user exists

        std::string homeDir = accountHandler.createUser(userName);

        // In this scenario, no directories should be created since the user exists
        assert(homeDir == expectedHomeDir);
        std::cout << "testCreateUserExisting: Passed" << std::endl;
    }
};

int main() {
    AccountHandlerTest tester;
    tester.runTests();
    return 0;
}
