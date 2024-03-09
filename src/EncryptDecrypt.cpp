#include "EncryptDecrypt.h"

CipherFS::KeyPair CipherFS::EncryptDecrypt::createKeyPair(const std::string& username) {

  KeyPair pair;
  pair.private_key_filename = username + "_keyfile.pem";
  pair.public_key_filename = username + "_public.pem";

  BIO *bp_public = NULL;
  BIO *bp_private = NULL;

  int ret = 0;
  int bits = 2048;
  
  unsigned long e = RSA_F4;
  RSA *rsa = NULL;
  BIGNUM *bne = NULL;




  try {
    
    bne = BN_new();
    ret = BN_set_word(bne, e);
    if (ret != 1) { 
      throw; 
    }

    rsa = RSA_new();
    ret = RSA_generate_key_ex(rsa, bits, bne, NULL);
    if (ret != 1) { 
      throw; 
    }

    bp_private = BIO_new_file(pair.private_key_filename.c_str(), "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL);
    if (ret != 1) { 
      throw; 
    }
    
    bp_public = BIO_new_file(pair.public_key_filename.c_str(), "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, rsa);
    if (ret != 1) { 
      throw; 
    }

  } catch (int errorCode) {
    std::cerr << "Error occured while generating RSA Key" << std::endl;
  }

  BN_free(bne);
  RSA_free(rsa);  
  BIO_free_all(bp_private);
  BIO_free_all(bp_public);  
  
  return pair;
}


bool CipherFS::EncryptDecrypt::checkPrivateKey(const std::string& privateKeyStr, const std::string& publicKeyStr) {
  const std::string secret = "secret";
  std::string res;
  try {
    res = decryptString(privateKeyStr, encryptString(publicKeyStr, secret));
  }
  catch(const std::exception& e) {    
  }
  return res == secret;
}

std::string CipherFS::EncryptDecrypt::decryptString(const std::string& private_key, const std::string& ciphertext) {
  if (ciphertext.length() == 0) {
    throw std::invalid_argument("Empty ciphertext");
  }
  
  BIO *pri_bio = BIO_new_mem_buf(private_key.c_str(), -1);
  if (!pri_bio) {
    BIO_free(pri_bio);
    throw std::invalid_argument("Invalid private key");
  }
  RSA *pri_rsa = PEM_read_bio_RSAPrivateKey(pri_bio, NULL, NULL, NULL);
  if (!pri_rsa) {
    throw std::invalid_argument("Invalid private key");
  }
  BIO_free(pri_bio);

  
  const unsigned char* ciphertext_char = reinterpret_cast<const unsigned char*>(&ciphertext[0]);
  unsigned char* plaintext_char  = new unsigned char[2048];
  int plaintext_length = RSA_private_decrypt(RSA_size(pri_rsa), ciphertext_char, plaintext_char, pri_rsa, RSA_PKCS1_OAEP_PADDING);
  if (plaintext_length == -1) {
    throw std::runtime_error("Decryption failed");
  } else {
    return std::string (reinterpret_cast<char*>(plaintext_char), plaintext_length);
  }
}


std::string CipherFS::EncryptDecrypt::retrieveFile(const std::string& private_key, const std::string& filepath) {
  std::ifstream infile(filepath);
  if (!infile) {
    std::cerr << "Error: Unable to open key file '" << filepath << "'" << std::endl;
    throw std::runtime_error("Invalid Filepath");
  }
  std::string ciphertext((std::istreambuf_iterator<char>(infile)),
                          std::istreambuf_iterator<char>());
  if (infile.fail() && !infile.eof()) {
    std::cerr << "Error: Failed to read file '" << filepath << "'" << std::endl;
    infile.close();
    throw std::runtime_error("Error while reading file");
  }
  infile.close();

  std::string plaintext = "";
  int CHUNK_SIZE = 256;
  for (int i=0; i < ciphertext.length(); i+= CHUNK_SIZE) {
    int remaining_bytes = ciphertext.length() - i;
    std::string ciphertext_chunk;
    if (remaining_bytes < CHUNK_SIZE) {
      ciphertext_chunk = ciphertext.substr(i, remaining_bytes);
    } else {
      ciphertext_chunk = ciphertext.substr(i, CHUNK_SIZE);
    }
    plaintext += decryptString(private_key, ciphertext_chunk);
  }
  return plaintext;
}




void CipherFS::EncryptDecrypt::secureFile(const std::string& public_key, const std::string& filepath) {
  std::ifstream infile(filepath);
  if (!infile) {
    std::cerr << "Error occured: Unable to open key file '" << filepath << "'" << std::endl;
    return;
  }
  std::string plaintext((std::istreambuf_iterator<char>(infile)),
                         std::istreambuf_iterator<char>());
  if (infile.fail() && !infile.eof()) {
    std::cerr << "Error: Failed to read file data '" << filepath << "'" << std::endl;
    infile.close();
    return;
  }
  infile.close();

  std::string ciphertext = "";
  int CHUNK_SIZE = 100;

  for (int i = 0; i < plaintext.length(); i+= CHUNK_SIZE) {
    int remaining_bytes = plaintext.length() - i;
    std::string plaintext_chunk;
    if (remaining_bytes < CHUNK_SIZE) {
      plaintext_chunk = plaintext.substr(i, remaining_bytes);
    } else {
      plaintext_chunk = plaintext.substr(i, CHUNK_SIZE);
    }
    ciphertext += encryptString(public_key, plaintext_chunk);
  }

  
  std::ofstream outfile(filepath, std::ios::trunc);
  if (!outfile) {
    std::cerr << "Error occured: Unable to open file '" << filepath << "'" << std::endl;
    return;
  }
  outfile << ciphertext;
  if (outfile.fail()) {
    std::cerr << "Error occured: Failed to write data to file'" << ciphertext << "'" << std::endl;
    outfile.close();
    return;
  }
  outfile.close();
  if (outfile.fail()) {
    std::cerr << "Error occured: Unable to close file '" << ciphertext << "'" << std::endl;
    return;
  }
}


std::string CipherFS::EncryptDecrypt::computeSha256(const std::string& inputstring) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, inputstring.c_str(), inputstring.size());
  SHA256_Final(hash, &sha256);
  std::stringstream sStream;
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
      sStream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return sStream.str();
}


std::string CipherFS::EncryptDecrypt::encryptString(const std::string& public_key, const std::string& plaintext) {
  
  if (plaintext.length() == 0) {
    throw std::invalid_argument("plaintext is empty");
  }
  
  BIO *pub_bio = BIO_new_mem_buf(public_key.c_str(), -1);
  
  if (!pub_bio) {
    BIO_free(pub_bio);
    throw std::invalid_argument("Invalid Pubilc Key");
  }

  RSA *pub_rsa = PEM_read_bio_RSAPublicKey(pub_bio, NULL, NULL, NULL);
  
  if (!pub_rsa) {
    throw std::invalid_argument("Invalid Pubilc Key");
  }

  BIO_free(pub_bio);

  const unsigned char* plaintext_char = reinterpret_cast<const unsigned char*>(plaintext.c_str());
  unsigned char* ciphertext_char  = new unsigned char[RSA_size(pub_rsa)];
  int ciphertext_length = RSA_public_encrypt(plaintext.size(), plaintext_char, ciphertext_char, pub_rsa, RSA_PKCS1_OAEP_PADDING);
  if (ciphertext_length == -1) {
    RSA_free(pub_rsa);
    delete[] ciphertext_char;
    throw std::runtime_error("Encryption failed");
  } else {
    return std::string (reinterpret_cast<char*>(ciphertext_char), ciphertext_length);
  }

}