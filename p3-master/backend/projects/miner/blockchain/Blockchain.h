#include <vector>
#include <fstream>
#include "Block.h"

using namespace std;
using namespace Json;
class Blockchain {
public:
    Blockchain(const string& jsonFile);
    void addBlock(Block &newBlock);
    uint32_t getLastIndex();
    vector<Block> getBlockchain();
    Block getLastBlock() const;
    uint32_t getBlockchainDifficulty();

    bool initializeBlockchain();
    void setBlockchainDifficulty(uint32_t difficulty);
    void saveBlockchain();
    int updateBlockVerification(uint blockIndex);
    bool validateBlock(Block &block);
    bool validateStudentData(const vector<int>& blockIds, const Value& data, int studentCode);
    bool validateCourseData(int blockId, Value &data);

    Value getStudentsInformation(Value data);
    Value getCoursesInformation(Value data);
    Value sendBlockchain(int lastBlock);

private:
    uint32_t _blockchainDifficulty;
    vector<Block> _blockchain;
    string _jsonFile;
};