#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "EncryptDecrypt.h"
#include <map>
#include <iostream>
#include <fstream>

namespace CipherFS {
    class FileDataStore {
    public:
        FileDataStore();
        ~FileDataStore();
        
        bool checkInitialization();
        void setupDataStore();
        std::vector<std::string> retrieveAllFilePaths();
        std::string registerFile(const std::string& path);
        std::string registerDirectory(const std::string& directoryPath);
        void removeFile(const std::string& path);
        std::string pathFromHash(const std::string& hashValue);
        std::string hashFromPath(std::string& path);
        bool checkFileExistence(const std::string& filePath);
        bool checkDirectoryExistence(const std::string& directoryPath);
    private:
        friend class DataStoreHelper;
        std::string mappingsFilename = "FileMappings.json";
        nlohmann::json fileMappingsData;

        std::map<std::string, nlohmann::json*> dataFile = {
            {this->mappingsFilename, &(this->fileMappingsData)}
        };

        void persistData();
        std::string encryptPathAndStore(const std::string& path);
    };

}
