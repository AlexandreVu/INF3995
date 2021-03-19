#include "ServerZmq.h"

static const char* ADD_BLOCK_FILTER = "NewBlock";
static const int TIMEOUT_SYNC = 1000;
static const string MINER_DOWN = "0"; 

ServerZmq::ServerZmq():
    _context(1),
    _publisher(_context, ZMQ_PUB),
    _synchronizationService(_context,ZMQ_ROUTER),
    _minersIds{MINER_DOWN,MINER_DOWN,MINER_DOWN}
{}

void ServerZmq::initialize() {
    _publisher.bind("tcp://*:5555");
    _synchronizationService.bind("tcp://*:5560");
    _synchronizationService.setsockopt(ZMQ_RCVTIMEO, TIMEOUT_SYNC);

    fprintf(stdout, "Démarrage du service Zmq! \n" );
}

int ServerZmq::synchronizeMiners(){
    int minersFound = 0;

    int parsedMiner;
    string bufferForEmptyFrame;
    string identity;
   
    for (int i = 0; i < NUMBER_MINERS; i++ ){
        if(receiveZMQ(_synchronizationService,identity)){
            receiveZMQ(_synchronizationService, bufferForEmptyFrame);
            receiveZMQ(_synchronizationService, parsedMiner);
            _minersIds[parsedMiner] = identity;
            
            sendmoreZMQ(_synchronizationService, identity);
            sendZMQ(_synchronizationService, "");

            minersFound++;
        } 
    }
    return minersFound;
}

bool ServerZmq::pushBlock(const Value& out){
    sendmoreZMQ(_publisher,ADD_BLOCK_FILTER);

    return sendZMQ(_publisher, out);
}

Value ServerZmq::requestInfo( const Value& request, int requestType, int minerNumber){
    if(minerNumber > NUMBER_MINERS - 1){
        throw REQUEST_MINER_ERROR;
    }
    if (requestType > NUMBER_SERVER_REQUESTS){
        throw REQUEST_TYPE_ERROR;
    }

    sendmoreZMQ(_synchronizationService, _minersIds[minerNumber]);
    sendZMQ(_synchronizationService, requestType, ZMQ_SNDMORE);
    sendZMQ(_synchronizationService, request);

    string identity;
    Value response;
    
    if (!receiveZMQ(_synchronizationService, identity)) {
        throw minerNumber;
    }
    receiveZMQ(_synchronizationService, response);

    return response;
}

