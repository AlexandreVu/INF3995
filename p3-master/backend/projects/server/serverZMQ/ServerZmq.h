#include <zmq.hpp>
#include <string.h>
#include <json/json.h>
#include "zmqMessageHandler.hpp"
#include "../../../src/requestType.hpp"

#define NUMBER_MINERS 3
#define NUMBER_SERVER_REQUESTS 6
#define REQUEST_TYPE_ERROR 10
#define REQUEST_MINER_ERROR 11

using namespace zmq;
using namespace std;
using namespace Json;

class ServerZmq {
    public:
        ServerZmq();
        void initialize();
        int synchronizeMiners();
        bool pushBlock(const Value& data);
        Value requestInfo(const Value& request, int requestType, int minerNumber);
    private:
        context_t _context;
        socket_t _publisher, _synchronizationService;
        string _minersIds[NUMBER_MINERS];
};