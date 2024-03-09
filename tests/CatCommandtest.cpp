#include <gtest/gtest.h>
#include "CatCommand.h"
#include "MockDataStoreHelper.h"
#include "MockStorageService.h"
#include <map>
#include <string>

using ::testing::Return;
using ::testing::_;

class CatCommandTest : public ::testing::Test {
protected:
    CipherFS::CatCommand command;
    std::map<std::string, std::string> params;
    MockDataStoreHelper database;
    MockStorageService filesystem;
    std::string username;
    std::string privateKey;
    std::string currentDir;

    virtual void SetUp() {
        username = "testUser";
        privateKey = "userPrivateKey";
        currentDir = "/home/testUser/";
        params["filename"] = "testFile.txt";
    }
};

TEST_F(CatCommandTest, FileDoesNotExist) {
    EXPECT_CALL(database, checkFileExistence(_)).WillOnce(Return(false));

    std::string output;
    std::streambuf* orig = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());

    command.execute(params, &database, &filesystem, username, privateKey, currentDir);

    std::cout.rdbuf(orig); // Reset to original
    EXPECT_NE(output.find("File does not exist"), std::string::npos);
}

TEST_F(CatCommandTest, FileReadSuccess) {
    EXPECT_CALL(database, checkFileExistence(_)).WillOnce(Return(true));
    EXPECT_CALL(filesystem, fetchFileContent(_, _)).WillOnce(Return("File content"));

    std::string output;
    std::streambuf* orig = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());

    command.execute(params, &database, &filesystem, username, privateKey, currentDir);

    std::cout.rdbuf(orig); // Reset to original
    EXPECT_EQ(output, "File content\n");
}

// Additional tests for Admin user, File read error handling, and Correct path construction

