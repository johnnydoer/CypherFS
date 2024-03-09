#include "DataStoreHelper.h"

namespace CipherFS {

DataStoreHelper::DataStoreHelper() {
    this->setupDataStore();
}

DataStoreHelper::~DataStoreHelper() {
    this->persistData();
}

void DataStoreHelper::addUser(const std::string& username) {
    if (username == "admin") {
        this->userDataStore.isAdminMode = true;
    } 
    
    this->userDataStore.userInfoData[username] = {};
    this->userDataStore.adminKeysData[username] = {};
    
    KeyPair key_pair = EncryptDecrypt::createKeyPair(username);
    std::ifstream ifs(key_pair.public_key_filename);
    std::string publicKeyContents((std::istreambuf_iterator<char>(ifs)),
                                  (std::istreambuf_iterator<char>()));
    
    this->userDataStore.userInfoData[username]["public_key"] = publicKeyContents;
    
    std::string rootDirHash = this->registerFile("/" + username + "/");
    std::string personalDirHash = this->registerFile("/" + username + "/personal/");
    std::string shareDirHash = this->registerFile("/" + username + "/shared/");
    
    this->userDataStore.userInfoData[username]["root_dir_hash"] = rootDirHash;
    this->userDataStore.userInfoData[username]["personal_dir_hash"] = personalDirHash;
    this->userDataStore.userInfoData[username]["share_dir_hash"] = shareDirHash;
    
    std::ifstream ifs2(key_pair.private_key_filename);
    std::string private_key_contents((std::istreambuf_iterator<char>(ifs2)),
                                   (std::istreambuf_iterator<char>()));
    this->userDataStore.adminKeysData[username] = private_key_contents;
    this->persistData();
}

std::vector<std::string> DataStoreHelper::listSharedFiles(const std::string& sourcePath) {
    std::string sourceHash = EncryptDecrypt::computeSha256(sourcePath);
    std::vector<std::string> filepaths = {};
    
    if (this->sharedDataStore.sharedInfoData.contains(sourceHash)) {
        std::vector<std::string> res = this->sharedDataStore.sharedInfoData[sourceHash].get<std::vector<std::string>>();
        for (std::string dst_filepath_hash : res) {
            std::string dst_filepath = this->pathFromHash(dst_filepath_hash);
            filepaths.push_back(dst_filepath);
        }
    }
    
    return filepaths;
}

bool DataStoreHelper::checkInitialization() {
    return  this->fileDataStore.checkInitialization() 
    && this->userDataStore.checkInitialization()
    && this->sharedDataStore.checkInitialization();
}

void DataStoreHelper::setupDataStore() {
    this->fileDataStore.setupDataStore();
    this->userDataStore.setupDataStore();
    this->sharedDataStore.setupDataStore();
}

void DataStoreHelper::persistData() {
    this->fileDataStore.persistData();
    this->userDataStore.persistData();
    this->sharedDataStore.persistData();
}

} // namespace CipherFS
