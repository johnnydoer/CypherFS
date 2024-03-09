#pragma once

#include <memory>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>
#define ASSERT assert

using std::unique_ptr;

namespace CipherFS {
  struct KeyPair {
    std::string public_key_filename;
    std::string private_key_filename;
  };
  class EncryptDecrypt {
    public:
      static KeyPair createKeyPair(const std::string& user);
      static std::string computeSha256(const std::string& data);
      static std::string encryptString(const std::string& publicKey, const std::string& plainText);
      static std::string decryptString(const std::string& privateKey, const std::string& cipherText);
      static bool checkPrivateKey(const std::string& privateKey, const std::string& publicKey);
      static void secureFile(const std::string& publicKey, const std::string& file);
      static std::string retrieveFile(const std::string& privateKey, const std::string& file);
  };
}
