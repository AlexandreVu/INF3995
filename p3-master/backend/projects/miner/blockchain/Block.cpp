#include "Block.h"

Block::Block(uint32_t indexIn, const string &dataIn, string previousHash):
    _index(indexIn),
    _data(dataIn),
    _previousHash(previousHash),
    _nonce(-1),
    _time(time(nullptr)),
    _verification(0)
{}

Block::Block() :
    _index(-1),
    _data(""),
    _previousHash(""),
    _nonce(-1),
    _time(time(nullptr)),
    _verification(0)
{}

uint32_t Block::getIndex() {
    return _index;
}

int64_t Block::getNonce() {
    return _nonce;
}

string Block::getData() {
    return _data;
}

string Block::getHash() {
    return _hash;
}

string Block::getPreviousHash() {
    return _previousHash;
}

time_t Block::getTime() {
    return _time;
}

int Block::getVerification() {
    return _verification;
}

void Block::setIndex(uint32_t index) {
    _index = index;
}

void Block::setNonce(int64_t nonce) {
    _nonce = nonce;
}

void Block::setData(const string &data) {
    _data = data;
}

void Block::setHash(const string &hash) {
    _hash = hash;
}

void Block::setPreviousHash(const string &previousHash) {
    _previousHash = previousHash;
}

void Block::setTime(time_t time) {
    _time = time;
}

void Block::setVerification(int verification) {
    _verification = verification;
}

string Block::mineBlock(uint32_t difficulty) {
    char prefixHash[difficulty+1];
    for(uint32_t i = 0; i < difficulty; i++) {
        prefixHash[i] = '0';
    }
    prefixHash[difficulty] = '\0';

    string initialString(prefixHash);

    do {
        _nonce++;
        _hash = createBlockHash(_index, _time, _data, _nonce, _previousHash);
    } while (_hash.substr(0, difficulty) != initialString);

    _updateVerification();
    return getHash();
}

void Block::blockIsVerified() {
    _updateVerification();
}

void Block::convertFromJSON(Json::Value data) {
    setIndex(data["index"].asUInt());
    setNonce(data["nonce"].asInt64());
    setData(data["data"].asString());
    setHash(data["hash"].asString());
    setPreviousHash(data["previousHash"].asString());
    setTime(data["time"].asUInt());
    setVerification(data["verification"].asInt());
}

Json::Value Block::convertToJSON() {
    Json::Value block;

    block["index"] = getIndex();
    block["nonce"] = getNonce();
    block["data"] = getData();
    block["hash"] = getHash();
    block["previousHash"] = getPreviousHash();
    block["time"] = (uint)getTime();
    block["verification"] = getVerification();
    
    return block;
}

void Block::_updateVerification() {
    _verification++;
}