#ifndef SHA256_HPP
#define SHA256_HPP

#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

static inline string createHashSha256(const string &str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream hashElements;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        hashElements << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return hashElements.str();
}

inline string createBlockHash(uint32_t index, time_t time, const string &data, int nonce, string previousHash){
    stringstream hashElements;
    hashElements << index << time << data << nonce << previousHash;
    return createHashSha256(hashElements.str());
}

#endif