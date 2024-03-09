#include "../include/EncryptDecrypt.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>

class EncryptionTests {
public:
    void runTests() {
        testCreateKeyPair();
        testComputeSha256();
        testEncryptDecryptString();
        testCheckPrivateKey();
        testSecureRetrieveFile();
    }

private:
    CipherFS::Encryption encryption;

    void testCreateKeyPair() {
        std::string user = "testUser";
        auto keyPair = encryption.createKeyPair(user);
        assert(!keyPair.publicKeyPath.empty() && !keyPair.privateKeyPath.empty());
        std::cout << "testCreateKeyPair: Passed" << std::endl;
    }

    void testComputeSha256() {
        std::string testData = "Hello, World!";
        auto hash = encryption.computeSha256(testData);
        assert(hash.length() == 64); // SHA256 hashes are 64 characters long
        std::cout << "testComputeSha256: Passed" << std::endl;
    }

    void testEncryptDecryptString() {
        std::string user = "testUser";
        auto keyPair = encryption.createKeyPair(user);
        std::string plaintext = "Hello, World!";
        auto encrypted = encryption.encryptString(keyPair.publicKeyPath, plaintext);
        auto decrypted = encryption.decryptString(keyPair.privateKeyPath, encrypted);
        assert(plaintext == decrypted);
        std::cout << "testEncryptDecryptString: Passed" << std::endl;
    }

    void testCheckPrivateKey() {
        std::string user = "testUser";
        auto keyPair = encryption.createKeyPair(user);
        bool isValid = encryption.checkPrivateKey(keyPair.privateKeyPath, keyPair.publicKeyPath);
        assert(isValid);
        std::cout << "testCheckPrivateKey: Passed" << std::endl;
    }

    void testSecureRetrieveFile() {
        std::string user = "testUser";
        auto keyPair = encryption.createKeyPair(user);
        std::string testFile = "testFile.txt";
        std::string content = "This is a test file.";

        // Write test file
        std::ofstream out(testFile);
        out << content;
        out.close();

        encryption.secureFile(keyPair.publicKeyPath, testFile);
        auto decryptedContent = encryption.retrieveFile(keyPair.privateKeyPath, testFile);

        // Clean up
        std::filesystem::remove(testFile);

        assert(content == decryptedContent);
        std::cout << "testSecureRetrieveFile: Passed" << std::endl;
    }
};

int main() {
    EncryptionTests tester;
    tester.runTests();
    return 0;
}
