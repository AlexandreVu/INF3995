#include "Blockchain.h"

using namespace std;

Blockchain::Blockchain(const string& jsonFile): _jsonFile(jsonFile) {
    _blockchain.emplace_back(Block(0, "Initial Block"));
    _blockchainDifficulty = 4;
}

void Blockchain::addBlock(Block &newBlock) {
    _blockchain.push_back(newBlock);
    saveBlockchain();
}

uint32_t Blockchain::getLastIndex() {
    return _blockchain.size();
}

vector<Block> Blockchain::getBlockchain() {
    return _blockchain;
}

Block Blockchain::getLastBlock() const {
    return _blockchain.back();                                      
}

uint32_t Blockchain::getBlockchainDifficulty() {
    return _blockchainDifficulty;
}

bool Blockchain::initializeBlockchain() {
    ifstream blockchainFile;
    blockchainFile.open(_jsonFile);
    if(blockchainFile.good()) {
        Value savedBlockchain;
        blockchainFile >> savedBlockchain;
        blockchainFile.close();
        Block block;
        for (int i = 1; i < savedBlockchain["blockchain"].size(); i++){
            block.convertFromJSON(savedBlockchain["blockchain"][i]);
            _blockchain.push_back(block);
        } 
        return true;
    } else {
        return false;
    }
}

void Blockchain::setBlockchainDifficulty(uint32_t difficulty) {
    _blockchainDifficulty = difficulty;
}

void Blockchain::saveBlockchain() {
    Value block;
    ofstream blockchainFile;
    blockchainFile.open(_jsonFile);
    blockchainFile << "{\"blockchain\": [" << endl;
    
    for (int i = 0; i < _blockchain.size(); i++) {
        block = _blockchain[i].convertToJSON();

        blockchainFile << block;
        if(i != _blockchain.size()-1) {
            blockchainFile << "," << endl;
        }
    }
    blockchainFile << "]}"<< endl;
    blockchainFile.close();
}

int Blockchain::updateBlockVerification(uint blockIndex){
    _blockchain[blockIndex].blockIsVerified();
    return _blockchain[blockIndex].getVerification();
}

bool Blockchain::validateBlock(Block &block) {
    string newHash = createBlockHash(block.getIndex(), block.getTime(), block.getData(),
                                      block.getNonce(), block.getPreviousHash());
    if (newHash.compare(block.getHash()) == 0) {
        block.blockIsVerified();
        block.setPreviousHash(getLastBlock().getHash());
        _blockchain.push_back(block);
        saveBlockchain();
        return true;
    } else {
        return false;
    }
}

bool Blockchain::validateCourseData(int blockId, Value &data) {
    Value blockInBlockchain;
    Reader decoder;
    FastWriter jsonWriter;
    
    decoder.parse(getBlockchain()[blockId].getData(), blockInBlockchain);
    blockInBlockchain["resultats"] = data["resultats"];

    string newHash = createBlockHash(getBlockchain()[blockId].getIndex(),
                                    getBlockchain()[blockId].getTime(),
                                    jsonWriter.write(blockInBlockchain),
                                    getBlockchain()[blockId].getNonce(),
                                    getBlockchain()[blockId].getPreviousHash());

    if (newHash.compare(getBlockchain()[blockId].getHash()) == 0) {
        return true;
    } else {
        return false;
    }
}

bool Blockchain::validateStudentData(const vector<int>& blockIds, const Value &data, int studentCode) {
    Value blockInBlockchain;
    Reader decoder;
    FastWriter jsonWriter;

    for (int i = 0; i < blockIds.size(); i++){
        decoder.parse(getBlockchain()[blockIds[i]].getData(), blockInBlockchain);
        blockInBlockchain["nom"] = data["resultats"][i]["nom"];
        blockInBlockchain["sigle"] = data["resultats"][i]["sigle"];
        blockInBlockchain["trimestre"] = data["resultats"][i]["trimestre"];

        for (int j = 0; j < blockInBlockchain["resultats"].size(); j++) {
            if (blockInBlockchain["resultats"][j]["matricule"].asInt() == studentCode) {
                blockInBlockchain["resultats"][j]["note"] = data["resultats"][i]["note"];
                break;
            }
        }

        string newHash = createBlockHash(getBlockchain()[blockIds[i]].getIndex(),
                                          getBlockchain()[blockIds[i]].getTime(),
                                          jsonWriter.write(blockInBlockchain),
                                          getBlockchain()[blockIds[i]].getNonce(),
                                          getBlockchain()[blockIds[i]].getPreviousHash());

        if (newHash.compare(getBlockchain()[blockIds[i]].getHash()) != 0) {
            return false;
        }
    }

    return true;
}

Value Blockchain::sendBlockchain(int lastBlock) {
    Value blockchainInJson, blocksToSend;
    ifstream blockchainFile;
    blockchainFile.open(_jsonFile);
    if(blockchainFile.good()) {
        blockchainFile >> blockchainInJson;
        blockchainFile.close();
        int index = 0;
        for (int i = lastBlock; i < blockchainInJson["blockchain"].size(); i++){
            blocksToSend["blockchain"][index++] = blockchainInJson["blockchain"][i];
        } 
    }

    return blocksToSend;
}