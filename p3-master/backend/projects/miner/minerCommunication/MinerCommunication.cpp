#include "MinerCommunication.h"

static const string FILTER = "NewBlock";

MinerCommunication::MinerCommunication(context_t& context):
    _context(context),
    _serverSubscription(_context, ZMQ_SUB),
    _minerReplier(_context, ZMQ_REP),
    _minerRequester(_context, ZMQ_DEALER),
    _synchronizeService(_context, ZMQ_DEALER),
    _pthreadConnection(_context, ZMQ_PAIR)
{}

void MinerCommunication::initialize(const Configuration& configuration) {
    _minerId = '0' + configuration.minerId;

    _serverSubscription.connect("tcp://localhost:" + to_string(configuration.portServer));
    _minerRequester.connect("tcp://localhost:" + to_string(configuration.portMiner2));
    _minerRequester.connect("tcp://localhost:" + to_string(configuration.portMiner3));
    _synchronizeService.connect("tcp://localhost:" + to_string(configuration.synchronizationServicePort));
    
    _minerReplier.bind("tcp://*:" + to_string(configuration.portMinerlocal));
    _pthreadConnection.bind("inproc://miningDone");

    _minerRequester.setsockopt(ZMQ_RCVTIMEO, 2000);
    _serverSubscription.setsockopt(ZMQ_SUBSCRIBE, FILTER.c_str(), FILTER.length());  
    setIdZMQ(_synchronizeService);

    _items[0] = { static_cast<void*>(_serverSubscription), 0, ZMQ_POLLIN, 0 };
    _items[1] = { static_cast<void*>(_minerReplier), 0, ZMQ_POLLIN, 0 };
    _items[2] = { static_cast<void*>(_synchronizeService), 0, ZMQ_POLLIN, 0 };
    _items[3] = { static_cast<void*>(_pthreadConnection), 0, ZMQ_POLLIN, 0 };

    synchronizeServer();
}

void MinerCommunication::synchronizeServer(){
    cout << "ZMQ : Attente du serveur..." << endl;

    string receiveBuffer;
    sendmoreZMQ(_synchronizeService, "");
    sendZMQ(_synchronizeService, _minerId); 
    receiveZMQ(_synchronizeService, receiveBuffer);

    cout << "ZMQ : Connection établie avec le serveur." << endl;
}

string MinerCommunication::getMinerId() const {
    return _minerId;
}

void MinerCommunication::pollSocketsEvents() const {
   zmq::poll(&_items[0], 4, 100);
}

bool MinerCommunication::isEvent(int socket) const {
    return _items[socket].revents & ZMQ_POLLIN;
}

bool MinerCommunication::sendThreadBlock(const Value& block){
    return sendZMQ(_pthreadConnection, block);
}

bool MinerCommunication::sendServerResponse(const Value &response) {
    return sendZMQ(_synchronizeService, response);
}

void MinerCommunication::sendThreadSynchro(){
    sendZMQ(_pthreadConnection, "");
}

void MinerCommunication::sendMinerResponse(const Value& response){
    sendZMQ(_minerReplier, response);
}

void MinerCommunication::sendMinerRequest(const struct Request& request, Value minerResponses[2]){
    for(int i = 0; i < 2; i++){   
        sendEmptyZMQ(_minerRequester);
        sendZMQ(_minerRequester,request.type, ZMQ_SNDMORE);
        sendZMQ(_minerRequester, request.data);
    }

    string emptyBuffer;
    for(int i = 0; i < 2; i++){
        Value frame;
        if(!receiveZMQ(_minerRequester, emptyBuffer)){
            cout << "ZMQ : Le délai d'attente avec le mineur " << i << " a été atteint." << endl;
        }else{
            receiveZMQ(_minerRequester, frame); 
        }
        minerResponses[i] = frame;
    }    
}

bool MinerCommunication::receiveThreadBlock(Value& MinedBlock){
    return receiveZMQ(_pthreadConnection, MinedBlock);
}

bool MinerCommunication::receiveServerRequest(struct Request& request ){
    int requestTypeMessage;
    Value requestData;
    
    bool isReceived = receiveZMQ(_synchronizeService, requestTypeMessage);
    receiveZMQ(_synchronizeService, requestData); 
    
    request.type = requestTypeMessage;
    request.data = requestData;

    return isReceived;
}

Value MinerCommunication::receiveBlockFromServer(){
    string envelopeBuffer;
    Value block;

    receiveZMQ(_serverSubscription, envelopeBuffer);
    receiveZMQ(_serverSubscription, block);

    return block;
}

void MinerCommunication::receiveMinerRequest(struct Request& request){
    Value message;
    int requestType;

    receiveZMQ(_minerReplier, requestType);
    receiveZMQ(_minerReplier, message);

    request.data = message;
    request.type = requestType;
}