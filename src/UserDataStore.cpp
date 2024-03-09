#include "UserDataStore.h"

namespace CipherFS {

// Constructor
UserDataStore::UserDataStore() {
    this->setupDataStore();
    std::ifstream filemapping(this->userInfoFilename);
    this->userInfoData = nlohmann::json::parse(filemapping);
}

// Destructor
UserDataStore::~UserDataStore() {
    this->persistData();
}

bool UserDataStore::checkInitialization() {
    return (
        std::filesystem::exists(this->userInfoFilename)
    );
}

void UserDataStore::setupDataStore() {
    if (!this->checkInitialization()) {
        for (auto const& [file, json] : this->dataFile) {
            std::ofstream output(file);
            output << nlohmann::json::object();
            output.close();
        } 
    }
}

void UserDataStore::setupAdminDataStore(const std::string& adminKey) {
    this->isAdminMode = true;
    
    if (!std::filesystem::exists(this->adminKeysFilename)) {
        std::ofstream output(this->adminKeysFilename);
        output << nlohmann::json::object();
        output.close();
        this->adminKeysData = nlohmann::json::object();
        return ;
    } 
    
    std::string decryptedFile = EncryptDecrypt::retrieveFile(adminKey, this->adminKeysFilename);
    this->adminKeysData = nlohmann::json::parse(decryptedFile);
}

bool UserDataStore::userExists(const std::string& user) {
   return this->userInfoData.contains(user);
}

std::string UserDataStore::fetchPublicKey(const std::string& user) {
    return this->fetchUserInfoProperty(user, "public_key");
}

std::string UserDataStore::identifyUserByPrivateKey(const std::string& key) {
    for (auto& [username, obj] : this->userInfoData.items()) {
        if (EncryptDecrypt::checkPrivateKey(key, obj["public_key"])) {
            return username;
        }
    }
    return "";
}

std::string UserDataStore::fetchUserInfoProperty(const std::string& user, const std::string& property) {
   try {
        return this->userInfoData[user][property].get<std::string>();
   } catch (const std::exception &ex) {
        return "";
    }
}

std::string UserDataStore::fetchPrivateKey(const std::string& user) {
    try {
       return this->adminKeysData[user].get<std::string>();
    } catch (const std::exception &ex) {
       return "";
    }
}

void UserDataStore::persistData() {
    for (auto const& [file, json] : this->dataFile) {
        std::ofstream output(file);
        output << std::setw(2) << *json << std::endl;
        output.close();
    }

    if (this->isAdminMode) {
        this->persistAdminData();
    }
}

void UserDataStore::persistAdminData() {
    std::ofstream output(this->adminKeysFilename);
    output << std::setw(2) << this->adminKeysData << std::endl;
    output.close();

    try {
        std::string adminPublicKey = this->fetchPublicKey("admin");
        std::string filePath = this->adminKeysFilename;
        EncryptDecrypt::secureFile(adminPublicKey, filePath);
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}

} // namespace CipherFS
