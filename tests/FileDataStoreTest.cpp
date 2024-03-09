#include "FileDataStore.h"
#include <iostream>
#include <cassert>

class FileDataStoreTest {
public:
    void runTests() {
        testCheckInitialization();
        testRegisterFile();
        testRetrieveAllFilePaths();
        testPathFromHash();
        testHashFromPath();
        testRemoveFile();
    }

private:
    void testCheckInitialization() {
        CipherFS::FileDataStore dataStore;
        bool initialized = dataStore.checkInitialization();
        std::cout << "testCheckInitialization: " << (initialized ? "Passed" : "Failed") << std::endl;
    }

    void testRegisterFile() {
        CipherFS::FileDataStore dataStore;
        std::string testFilePath = "testFile.txt"; // Adjust accordingly
        std::string hash = dataStore.registerFile(testFilePath);
        assert(!hash.empty());
        std::cout << "testRegisterFile: Passed" << std::endl;
    }

    void testRetrieveAllFilePaths() {
        CipherFS::FileDataStore dataStore;
        dataStore.registerFile("testFile.txt"); // Make sure to register a file first
        auto paths = dataStore.retrieveAllFilePaths();
        assert(!paths.empty());
        std::cout << "testRetrieveAllFilePaths: Passed" << std::endl;
    }

    void testRemoveFile() {
        CipherFS::FileDataStore dataStore;
        std::string testFilePath = "testFile.txt"; // Adjust accordingly
        dataStore.registerFile(testFilePath);
        dataStore.removeFile(testFilePath);
        auto paths = dataStore.retrieveAllFilePaths();
        assert(paths.empty());
        std::cout << "testRemoveFile: Passed" << std::endl;
    }

    void testPathFromHash() {
        CipherFS::FileDataStore dataStore;
        std::string testFilePath = "testFile.txt"; // Adjust accordingly
        std::string hash = dataStore.registerFile(testFilePath);
        std::string path = dataStore.pathFromHash(hash);
        assert(path == testFilePath);
        std::cout << "testPathFromHash: Passed" << std::endl;
    }

    void testHashFromPath() {
        CipherFS::FileDataStore dataStore;
        std::string testFilePath = "testFile.txt"; // Adjust accordingly
        std::string hash = dataStore.hashFromPath(testFilePath);
        assert(!hash.empty());
        std::cout << "testHashFromPath: Passed" << std::endl;
    }
};

int main() {
    FileDataStoreTest tester;
    tester.runTests();
    return 0;
}
