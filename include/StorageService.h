#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <exception>

#include "EncryptDecrypt.h" 
#include "Utils.h"

#define FILESYSTEM_DIRECTORY "FS_STORAGE"

namespace CipherFS {
  class StorageService {
  public:
    StorageService();
    std::string fetchFileContent(const std::string& virtualPath, const std::string& decryptionKey);
    bool establishDirectory(const std::string& virtualDirectoryPath);
    bool generateFile(const std::string& virtualFilePath, const std::string& data, const std::string& encryptionKey);
    bool replicateFile(const std::string& sourceVirtualPath, const std::string& destinationVirtualPath, const std::string& sourceDecryptionKey, const std::string& destinationEncryptionKey);
    bool removeFile(const std::string& virtualFilePath);

    struct FileReadError : std::exception {
      std::string message;
      FileReadError(const std::string& errMsg) : message(errMsg) {}
      const char* what() const noexcept override {
        return message.c_str();
      }
    };

    struct DirectoryCreationError : std::exception {
      std::string message;
      DirectoryCreationError(const std::string& errMsg) : message(errMsg) {}
      const char* what() const noexcept override {
        return message.c_str();
      }
    };

    struct FileCreationError : std::exception {
      std::string message;
      FileCreationError(const std::string& errMsg) : message(errMsg) {}
      const char* what() const noexcept override {
        return message.c_str();
      }
    };

    struct FileCopyError : std::exception {
      std::string message;
      FileCopyError(const std::string& errMsg) : message(errMsg) {}
      const char* what() const noexcept override {
        return message.c_str();
      }
    };

    struct FileDeletionError : std::exception {
      std::string message;
      FileDeletionError(const std::string& errMsg) : message(errMsg) {}
      const char* what() const noexcept override {
        return message.c_str();
      }
    };

  private:
    std::filesystem::path basePath = std::filesystem::current_path() / FILESYSTEM_DIRECTORY;
    std::filesystem::path translateVirtualToPhysicalPath(const std::string& virtualPath);
  };
}
