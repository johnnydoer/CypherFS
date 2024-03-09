#pragma once

#include <iostream>

#include <DataStoreHelper.h>
#include <Utils.h>
#include <StorageService.h>

namespace CipherFS {
  class AccountHandler {
    public:
      AccountHandler(DataStoreHelper* database);
      std::string createUser(const std::string& userName);

    private:
      DataStoreHelper* database;
      StorageService storage;
  };
}