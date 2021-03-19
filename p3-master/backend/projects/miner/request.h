#ifndef REQUEST_H
#define REQUEST_H
#include <json/json.h>

using namespace Json;

#define MINING_DONE 4
#define VALIDATE_BLOCK 5
#define UPDATE_BLOCKCHAIN 6
#define UPDATE_VERIFICATION_STATUS 7

struct Request{
    int type;
    Value data;
    Request(int type = 0, Value data = Value()): type(type), data(data)
    {}
};

#endif