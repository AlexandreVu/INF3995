#include <zmq.hpp>
#include <string.h>
#include <json/json.h>
#include "ReadConfig.hpp"
#include "zmqMessageHandler.hpp"
#include "../request.h"

#define SERVER_PUSH 0
#define MINER_REQUEST 1
#define SERVER_REQUEST 2
#define PTHREAD_DONE 3

using namespace zmq;
using namespace std;
using namespace Json;

class MinerCommunication {
    public:
        MinerCommunication(context_t& context);
        void initialize(const Configuration& configuration);
        void synchronizeServer();
        string getMinerId() const;

        void pollSocketsEvents() const;
        bool isEvent(int socketNumber) const;

        bool sendThreadBlock(const Value& blockData);
        bool sendServerResponse(const Value& response);
        void sendThreadSynchro();
        void sendMinerResponse(const Value& response);
        void sendMinerRequest(const struct Request& request, Value minerResponses[2]);

        bool receiveThreadBlock(Value& MinedBlock);
        bool receiveServerRequest(struct Request& request);
        Value receiveBlockFromServer();
        void receiveMinerRequest(struct Request& request);

    private:
        context_t& _context;
        socket_t _serverSubscription, _minerReplier, _minerRequester, _synchronizeService, _pthreadConnection;
        pollitem_t _items[4];
        string _minerId;
};