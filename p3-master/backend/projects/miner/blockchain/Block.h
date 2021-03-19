#include <cstdint>
#include <iostream>
#include <sstream>
#include <json/json.h>
#include "jwt/jwt_all.h"
#include "../hashCreation/sha256.hpp"

using namespace std;

class Block {
public:

    Block(uint32_t indexIn, const string &dataIn, string previousHash = "");
    Block();
    uint32_t getIndex();
    int64_t getNonce();
    string getData();
    string getHash();
    string getPreviousHash();
    time_t getTime();
    int getVerification();

    void setIndex(uint32_t index);
    void setNonce(int64_t nonce);
    void setData(const string &data);
    void setHash(const string &hash);
    void setPreviousHash(const string &previousHash);
    void setTime(time_t time);
    void setVerification(int verification);

    string mineBlock(uint32_t difficulty);
    void blockIsVerified();

    void convertFromJSON(Json::Value data);
    Json::Value convertToJSON();
    
private:
    uint32_t _index;
    int64_t _nonce;
    string _data;
    string _hash;
    string _previousHash;
    time_t _time;
    int _verification;

    void _updateVerification();
};