#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "EncryptDecrypt.h"
#include <map>
#include <iostream>
#include <fstream>

namespace CipherFS {
    class UserDataStore {
    public:
        UserDataStore();
        ~UserDataStore();
        
        bool checkInitialization();
        void setupDataStore();
        void setupAdminDataStore(const std::string& adminKey);
        bool userExists(const std::string& user);
        std::string fetchPublicKey(const std::string& user);
        std::string identifyUserByPrivateKey(const std::string& key);
        std::string fetchUserInfoProperty(const std::string& user, const std::string& property);
        std::string fetchPrivateKey(const std::string& user);
        
    private:
        friend class DataStoreHelper;
        bool isAdminMode = false;
        std::string userInfoFilename = "UserInfo.json";
        std::string adminKeysFilename = "AdminKeys.json";
        nlohmann::json userInfoData;
        nlohmann::json adminKeysData;

        std::map<std::string, nlohmann::json*> dataFile = {
            {this->userInfoFilename, &(this->userInfoData)}
        };

        void persistData();
        void persistAdminData();
    };
}
