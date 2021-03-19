#include <thread>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <restbed>
#include <string>
#include <iostream>
#include <json/json.h>
#include <fstream>
#include "RestAPI/RestAPI.h"
#include "zmqSingelton.h"
#include <fstream>
#include <unistd.h>

using namespace std;
using namespace restbed;
using namespace Json;

static void testZMQ();
static const int INITIAL_MINING_DIFFICULTY = 4;
void setInitialMiningDifficulty(int difficulty);

void service_ready_handler(Service &)
{
    fprintf(stderr, "Démarrage de l'api REST! \n");
}

int main(const int, const char **)
{
    zmqBroker.initialize();

    initializeDB();
    initializeAdmin(ANDROID_DB);
    
    uint8_t nMiners;
    while((nMiners = zmqBroker.synchronizeMiners()) == 0)
        cout << "Aucun mineur up, nouvelle tentative de connexion.\n";
    fprintf(stdout, "Le server démarre avec %d mineurs up.\n", nMiners);

    setInitialMiningDifficulty(INITIAL_MINING_DIFFICULTY);

    // -------------------------- Ressources pour l'api de l'appli pc -----------------------------
    auto resourcePostAdminLogin = make_shared<Resource>();
    resourcePostAdminLogin->set_path("/admin/login");
    resourcePostAdminLogin->set_method_handler("POST", login_handler);

    auto resourcePostAdminLogout = make_shared<Resource>();
    resourcePostAdminLogout->set_path("/admin/logout");
    resourcePostAdminLogout->set_method_handler("POST", logout_handler);

    auto resourcePostAdminChangePassword = make_shared<Resource>();
    resourcePostAdminChangePassword->set_path("/admin/motdepasse");
    resourcePostAdminChangePassword->set_method_handler("POST", change_password_handler);

    auto resourceGetAdminChaine = make_shared< Resource >();
    resourceGetAdminChaine->set_path("/admin/chaine/{id: [1-3]*}");
    resourceGetAdminChaine->set_method_handler("POST", admin_blockchain);

    auto resourceGetAdminLogsMiners = make_shared< Resource >();
    resourceGetAdminLogsMiners->set_path("/admin/logs/{id: [1-3]*}");
    resourceGetAdminLogsMiners->set_method_handler("POST", admin_logs);

    auto resourceGetAdminLogsServer = make_shared< Resource >();
    resourceGetAdminLogsServer->set_path("/admin/logs/serveurweb");
    resourceGetAdminLogsServer->set_method_handler("POST", admin_logs);

    auto resourcePostAdminCreationCompte = make_shared< Resource >();
    resourcePostAdminCreationCompte->set_path("/admin/creationcompte");
    resourcePostAdminCreationCompte->set_method_handler("POST", admin_creation_compte);

    auto resourcePostAdminSuppressionCompte = make_shared< Resource >();
    resourcePostAdminSuppressionCompte->set_path("/admin/suppressioncompte");
    resourcePostAdminSuppressionCompte->set_method_handler("POST", admin_suppression_compte);
    // -----------------------------------------------------------------------------------------------

    // ------------------------- Ressources pour l'api de l'appli android ----------------------------
    auto resourcePostUsagerLogin = make_shared<Resource>();
    resourcePostUsagerLogin->set_path("/usager/login");
    resourcePostUsagerLogin->set_method_handler("POST", login_handler);

    auto resourcePostUsagerLogout = make_shared<Resource>();
    resourcePostUsagerLogout->set_path("/usager/logout");
    resourcePostUsagerLogout->set_method_handler("POST", logout_handler);

    auto resourcePostUsagerChangePassword = make_shared<Resource>();
    resourcePostUsagerChangePassword->set_path("/usager/motdepasse");
    resourcePostUsagerChangePassword->set_method_handler("POST", change_password_handler);

    auto resourcePostTransaction = make_shared<Resource>();
    resourcePostTransaction->set_path("/transaction");
    resourcePostTransaction->set_method_handler("POST", transaction_handler);

    auto resourceGetInfoCours = make_shared<Resource>();
    resourceGetInfoCours->set_path("/info/cours");
    resourceGetInfoCours->set_method_handler("POST", info_cours_handler);

    auto resourceGetInfoEtudiant = make_shared<Resource>();
    resourceGetInfoEtudiant->set_path("/info/etudiant");
    resourceGetInfoEtudiant->set_method_handler("POST", info_etudiant_handler);

    auto resourceGetFichierNotes = make_shared<Resource>();
    resourceGetFichierNotes->set_path("/fichier/notes");
    resourceGetFichierNotes->set_method_handler("POST", fichier_notes_handler);
    // ------------------------------------------------------------------------------------------------

    auto settings = make_shared<Settings>();
    settings->set_port(8080);

    auto service = make_shared<Service>();

    service->publish(resourcePostAdminLogin);
    service->publish(resourcePostAdminLogout);
    service->publish(resourcePostAdminChangePassword);
    service->publish(resourceGetAdminChaine);
    service->publish(resourceGetAdminLogsMiners);
    service->publish(resourceGetAdminLogsServer);
    service->publish(resourcePostAdminCreationCompte);
    service->publish(resourcePostAdminSuppressionCompte);

    service->publish(resourcePostUsagerLogin);
    service->publish(resourcePostUsagerLogout);
    service->publish(resourcePostUsagerChangePassword);
    service->publish(resourcePostTransaction);
    service->publish(resourceGetInfoCours);
    service->publish(resourceGetInfoEtudiant);
    service->publish(resourceGetFichierNotes);

    service->set_ready_handler(service_ready_handler);
    service->start(settings);
    
    return EXIT_SUCCESS;
}

void setInitialMiningDifficulty(int difficulty){
    Value miningDifficultyReq;
    miningDifficultyReq["difficulty"] = difficulty;

    for(int i = 0; i < 3; i++){
        try{
            zmqBroker.requestInfo(miningDifficultyReq, MINING_DIFFICULTY, i);
        } catch(int exception){
            cout << "miner" << i << " down" << endl;
        }
    
    }
}

static void testZMQ() {
    ifstream fileStream("../../../projects/server/MockRequest.json");
    ifstream fileStream1("../../../projects/server/MockRequest2.json");
    ifstream fileStream2("../../../projects/server/MockRequest3.json");

    Reader reader;
    //modifier pour les demandes de requetes du server
    Value objetJson;
    Value objetJson2;
    Value objetJson3;

    reader.parse(fileStream, objetJson);   
    reader.parse(fileStream1, objetJson2);   
    reader.parse(fileStream2, objetJson3);   
    
    cout << "TEST ZMQ:" << endl << "Pushing a block ..." << endl;
    zmqBroker.pushBlock(objetJson);
    sleep(2);
    zmqBroker.pushBlock(objetJson2);
    sleep(2);
    zmqBroker.pushBlock(objetJson3);

    sleep(5);
    int minerNumber = 0;
    StyledWriter writer;

    Value miningDiffiultyReq;
    miningDiffiultyReq["difficulty"] = 6;

    Value getCoursReq;
    getCoursReq["trimestre"] = 20193;
    getCoursReq["sigle"] = "LOG2990";

    Value getEtudiantReq;
    getEtudiantReq["matricule"] = 2143765;
    getEtudiantReq["trimestre"] = 20193;
    getEtudiantReq["sigle"] = "INF3995";

    Value getBlockChain;
    getBlockChain["dernier_blocs"] = 2;

    Value getMinerLogs;
    getMinerLogs["dernier_log"] = 0;
    try {
        cout << "Test setting mining difficulty:" << endl;
        cout << writer.write(zmqBroker.requestInfo(miningDiffiultyReq, MINING_DIFFICULTY, minerNumber)) << endl;
        minerNumber = ++minerNumber % 3;
    
        cout << "Test get course information:" << endl;
        cout << writer.write(zmqBroker.requestInfo(getCoursReq, GET_INFO_COURS, minerNumber)) << endl;
        minerNumber = ++minerNumber % 3;

        cout << "Test get course etudiant:" << endl;
        cout << writer.write(zmqBroker.requestInfo(getEtudiantReq, GET_INFO_ETUDIANT, minerNumber)) << endl;
        minerNumber = ++minerNumber % 3;

        cout << "Test get blockchain:" << endl;
        cout << writer.write(zmqBroker.requestInfo(getBlockChain, GET_BLOCKCHAIN_FILE, minerNumber)) << endl;
        minerNumber = ++minerNumber % 3;

        cout << "Test get miner logs:" << endl;
        cout << writer.write(zmqBroker.requestInfo(getMinerLogs, GET_MINER_LOGS, minerNumber)) << endl;
    } catch (int error){
        cout << "ZMQ request error: ";
        cout << error << endl;
    }
}