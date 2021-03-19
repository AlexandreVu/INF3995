#ifndef _WIN32
#include <unistd.h>
#endif
#include "../database/MinerDbManager.cpp"
#include <zmq.hpp>
#include "minerCommunication/MinerCommunication.h"
#include "ReadConfig.hpp"
#include "../../src/requestType.hpp"
#include "request.h"
#include "BlockchainSingleton.hpp"
#include <pthread.h>
#include <signal.h>
#include "minerTests.hpp"

#define DEFAULT_FILE_PATH "../../../portConfigurations/config1.conf"

pthread_t minerThread;
const char* databaseFilename = "BlockchainInformation.db";

using namespace std;

void* miningRoutine(void *arg);

static int64_t nonce = 0;
bool isMining = false;

int main(int argc, char* argv[]) {
    context_t context(1);

    string configurationFilePath = DEFAULT_FILE_PATH;
    if(argc > 1){
        configurationFilePath = argv[1];
    }

    Configuration configuration;
    if (!configure(configurationFilePath, configuration)){
        logsManager.appendLog("ERREUR", "Fichier de configuration introuvable!");
        exit(1);
    }
    nonce = configuration.minerNonce;
    MinerCommunication communicator(context);

    blockchain.initializeBlockchain();
    cout << "Blockchain : Fichier de blockchain chargé" << endl;
 
    initializeBlockchainDatabase(databaseFilename);

    try{
        communicator.initialize(configuration);
        logsManager.appendLog("INFO", "La communication entre les composantes est operationnelle!");
        cout << "ZMQ : le service est opérationnel!" << endl;
    } catch(const exception &e) {
        logsManager.appendLog("ERREUR", "Erreur lors de l'initialisation des composantes!");
        cout << "ZMQ : erreur lors de l'initialisation!" << endl; 
        exit(1);
    }

    while(1){
        communicator.pollSocketsEvents();

        if(communicator.isEvent(SERVER_PUSH)) {
            FastWriter jsonWriter;
            blockInformation = communicator.receiveBlockFromServer();
            if(!isMining) {
                arrangeBlockJson(blockInformation["transaction"]);
                Block newBlock(blockchain.getLastIndex(), jsonWriter.write(blockInformation["transaction"]), blockchain.getLastBlock().getHash());
                isMining = true;
                pthread_create(&minerThread, NULL, miningRoutine, (void*) &context);
                communicator.sendThreadBlock(newBlock.convertToJSON());
            }else{
                logsManager.appendLog("ERREUR", "Ajout d'information echoue, l'ajout precedent est non complete.");
                cout << "Minage : impossible de lancer un minage avant que le minage précédent ne soit terminé" << endl;
            }
        }
        if(communicator.isEvent(MINER_REQUEST)) {
            Request request;
            Value response;
            communicator.receiveMinerRequest(request);
            if(request.type == VALIDATE_BLOCK && isMining){
                cout << "Bloc : Le bloc en cours de minage ne sera pas utilisé." << endl;
                communicator.sendThreadSynchro();
            }
            response = handleMinerRequest(request);
            communicator.sendMinerResponse(response);
        }
        if(communicator.isEvent(SERVER_REQUEST)){
            Request request;
            communicator.receiveServerRequest(request);
            communicator.sendServerResponse(handleServerRequest(request));
        }
        if(communicator.isEvent(PTHREAD_DONE)){
            Value minedBlock;
            Value minersResponses[2];

            communicator.receiveThreadBlock(minedBlock);

            Value requestData;
            requestData["block"] = minedBlock;
            requestData["minerNumber"] = communicator.getMinerId();
            communicator.sendMinerRequest(Request(VALIDATE_BLOCK,requestData), minersResponses);

            Block block;
            block.convertFromJSON(minedBlock);

            int verificationStatus = 1;
            for(int i = 0; i < 2; i++) {
                if(minersResponses[i]["isValid"].asBool()){
                    verificationStatus++;
                    block.blockIsVerified();
                }
            }

            if(verificationStatus == 3){
                Value indexBlock, minerResponses[2];
                indexBlock["index"] = block.getIndex();
    
                communicator.sendMinerRequest(Request(UPDATE_VERIFICATION_STATUS, indexBlock), minerResponses);
            }
            
            blockchain.addBlock(block);
            insertInDatabase(blockToSave);
        }
    }
    return 0;
}

void* miningRoutine(void* arg){
    context_t* context = static_cast<context_t*>(arg);
    
    socket_t sender(*context, ZMQ_PAIR);
    sender.connect("inproc://miningDone");

    Value blockData;

    receiveZMQ(sender, blockData);

    Block newBlock;    
    newBlock.convertFromJSON(blockData);

    newBlock.setNonce(nonce);
    string hash = newBlock.mineBlock(blockchain.getBlockchainDifficulty());
    logsManager.appendLog("INFO", "Nouveau bloc d'information cree avec le numero: " + to_string(newBlock.getIndex()));
    cout << "Bloc : nouveau bloc miné avec le hash : " + hash << endl;

    string stop;
    if(!receiveZMQ(sender, stop, ZMQ_DONTWAIT)){
        blockToSave = blockInformation["transaction"];
        sendZMQ(sender, newBlock.convertToJSON()); 
    }
    isMining = false;
    sender.close();
    
    return NULL;
}