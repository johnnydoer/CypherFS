#include "CdCommand.h"
#include <iostream>

namespace CipherFS {

void CdCommand::execute(std::map<std::string, std::string>& params, 
                        DataStoreHelper* database, StorageService* filesystem, 
                        std::string& username, std::string& privateKey, std::string& currentDir) {
    
  std::string targetDir = params["targetDir"];

  // Handle root and current directory cases directly
  if (targetDir == "/") {
    currentDir = "/" + username + "/";
    return;
  }

  if (targetDir == ".") {
    return; // No change for current directory
  }

  // Handle moving up in the directory structure
  if (targetDir == "..") {
    return moveToParentDirectory(username, currentDir);
  }

  // Handle general directory change
  changeDirectory(targetDir, username, database, currentDir);
}

void CdCommand::moveToParentDirectory(const std::string& username, std::string& currentDir) {
  if (currentDir == "/" + username + "/") {
    if (username != "admin") {
      std::cout << "Forbidden" << std::endl;
    }
    return;
  }

  if (currentDir != "/") {
    size_t lastSlashPos = currentDir.find_last_of('/', currentDir.length() - (currentDir.back() == '/' ? 2 : 1));
    currentDir = currentDir.substr(0, lastSlashPos + 1);
  }
}

void CdCommand::changeDirectory(const std::string& targetDir, const std::string& username, 
                                DataStoreHelper* database, std::string& currentDir) {
  std::string tempDir = buildTempDir(targetDir, currentDir);

  if (username != "admin" && (Utils::fetchOwner(tempDir) != username || !database->checkDirectoryExistence(tempDir))) {
    std::cout << "Forbidden or Target directory does not exist" << std::endl;
    return;
  }

  if (username == "admin" && !database->checkDirectoryExistence(tempDir) && tempDir != "/") {
    std::cout << "Target directory doesn't exist" << std::endl;
    return;
  }

  currentDir = tempDir;
}

std::string CdCommand::buildTempDir(const std::string& targetDir, const std::string& currentDir) {
  std::vector<std::string> dirs = Utils::split(targetDir, '/');
  std::string tempDir = currentDir;

  for (const auto& dir : dirs) {
    if (dir.empty()) continue; // Skip empty segments, which occur due to consecutive slashes
    if (dir == "..") {
      size_t lastSlashPos = tempDir.find_last_of('/', tempDir.length() - (tempDir.back() == '/' ? 2 : 1));
      tempDir = tempDir.substr(0, lastSlashPos + 1);
    } else {
      tempDir += dir + "/";
    }
  }

  return tempDir;
}

} // namespace CipherFS
