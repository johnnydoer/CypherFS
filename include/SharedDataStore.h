#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "EncryptDecrypt.h"
#include <map>
#include <iostream>
#include <fstream>

namespace CipherFS {
    class SharedDataStore {
    public:
        SharedDataStore();
        ~SharedDataStore();

        bool checkInitialization();
        void setupDataStore();
        void shareFile(const std::string& sourcePath, const std::string& destinationPath);
    private:
        friend class DataStoreHelper;
        std::string sharedInfoFilename = "SharedInfo.json";
        nlohmann::json sharedInfoData;

        std::map<std::string, nlohmann::json*> dataFile = {
            {this->sharedInfoFilename, &(this->sharedInfoData)}
        };

        void persistData();
    };
}
