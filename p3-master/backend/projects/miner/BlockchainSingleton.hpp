#ifndef BLOCKCHAINSINGLETON_HPP
#define BLOCKCHAINSENGLETON_HPP
#include "blockchain/Blockchain.h"
#include "../server/LogsManager/LogsManager.cpp"

static Blockchain blockchain("blockchain.json");
LogsManager logsManager("logs.json");

Json::Value blockInformation, blockToSave;
char *courseNumber;
list<Json::Value> classList, studentList;

void arrangeBlockJson(Value &blockData) {
    for (int i = 0; i < blockData["resultats"].size(); i++) {
        for (int j = i + 1; j < blockData["resultats"].size(); j++) {
            if (blockData["resultats"][i]["matricule"] > blockData["resultats"][j]["matricule"]) {
                Value temp = blockData["resultats"][i];
                blockData["resultats"][i] = blockData["resultats"][j];
                blockData["resultats"][j] = temp;
            }
        }
    }
}

void insertInDatabase(const Value &blockchainData) {
    
    int previousBlockId = selectBlockIdFromCourses(strdup(blockchainData["sigle"].asString().c_str()), 
                                                    stoi(blockchainData["trimestre"].asString()));                                              
    if (previousBlockId != NULL)
    {
        cout << "Ce bloc existe déjà, ancienne version supprimée." << endl;
        deleteBlockchainData(previousBlockId);
    }
   
    for (int i = 0; i < blockchainData["resultats"].size(); i++) {
        insertBlockchainData(stoi(blockchainData["resultats"][i]["matricule"].asString()), 
                            strdup(blockchainData["resultats"][i]["prenom"].asString().c_str()), 
                            strdup(blockchainData["resultats"][i]["nom"].asString().c_str()), 
                            strdup(blockchainData["sigle"].asString().c_str()),
                            stoi(blockchainData["trimestre"].asString()), 
                            strdup(blockchainData["nom"].asString().c_str()), 
                            strdup(blockchainData["resultats"][i]["note"].asString().c_str()), 
                            blockchain.getLastBlock().getIndex());
    }
    insertPdfInCourseTable(strdup(blockInformation["pdf"].asString().c_str()), 
                        strdup(blockchainData["sigle"].asString().c_str()), 
                        stoi(blockchainData["trimestre"].asString()));
    logsManager.appendLog("INFO", "Informations du cours " + blockchainData["sigle"].asString() + 
                         " pour le trimestre " + blockchainData["trimestre"].asString() + " sauvegardees dans la base de donnees!");
    cout << "Bloc sauvegardé dans la base de données!" << endl;
}

Value handleMinerRequest(const Request& request){
    Value response;
  
    switch(request.type){
        case VALIDATE_BLOCK:
        { 
            Block block;
            block.convertFromJSON(request.data["block"]);
            response["isValid"] = blockchain.validateBlock(block);
            
            if (response["isValid"].asBool()) {
                cout << "Blockchain : bloc valide reçu du mineur " + request.data["minerNumber"].asString() + "." << endl;
                Reader reader;
                Value dataToSave;
                reader.parse(request.data["block"]["data"].asString(), dataToSave);
                insertInDatabase(dataToSave);
            }else{
                logsManager.appendLog("ATTENTION", "Information non validee par le mineur" );
                cout << "Blockchain : bloc non valide reçu du mineur " + request.data["minerNumber"].asString() + "."<< endl;
            }

            break;
        }
        case UPDATE_VERIFICATION_STATUS:
        {
            int index = request.data["index"].asUInt64();

            int newVerificationStatus = blockchain.updateBlockVerification(index);
            cout << "Blockchain : Mise à jour du statut de vérification du bloc " + 
                    to_string(index) + " à " + to_string(newVerificationStatus) +  "!"<< endl;
            blockchain.saveBlockchain();
            break;
        }
    }
    return response;
}

Value handleServerRequest(const Request& request){
    Value response;
    switch(request.type){
        case MINING_DIFFICULTY:
            {
                int difficulty = request.data["difficulty"].asInt();
                blockchain.setBlockchainDifficulty(difficulty);
                cout <<  "Blockchain : Difficulté de minage changée à " + to_string(difficulty) << endl;
                response["difficulty"] = request.data;
                break;
            }
        case GET_INFO_COURS:
            {
                string courseNumber = request.data["sigle"].asString();
                int trimestre = stoi(request.data["trimestre"].asString());

                studentList = selectStudentsFromCourseNumber(strdup(courseNumber.c_str()), trimestre);
                int index = 0;

                if(!studentList.empty()) {
                    response["nom"] = courseName;
                    courseName.erase();
                }

                for(std::list<Json::Value>::iterator it = studentList.begin(); it != studentList.end(); ++it) {
                    response["resultats"][index]["nom"] = (*it)["nom"];
                    response["resultats"][index]["prenom"] = (*it)["prenom"];
                    response["resultats"][index]["matricule"] = (*it)["matricule"];
                    response["resultats"][index]["note"] = (*it)["note"];

                    index++;
                }

                if(!response.empty()) {
                    int blockId = selectBlockIdFromCourses(strdup(courseNumber.c_str()), trimestre);

                    if (!blockchain.validateCourseData(blockId, response)) {
                        logsManager.appendLog("ATTENTION", "Information corrompue dans la base de donnees pour le cours " + courseNumber + "!");
                        cout << "Blockchain : l'information de la base de données n'est pas valide pour le cours " + courseNumber + "!" << endl;
                        response["resultats"] = Value();
                    } else {
                        cout << "Blockchain : Information de la base de données valide!" << endl;
                    }
                } else {
                    logsManager.appendLog("ERREUR", "L'information recherchee n'est pas dans la base de donnees!");
                    cout << "L'information recherchée n'est pas dans la base de données!" << endl;
                }

                break;
            }
        case GET_INFO_ETUDIANT:
            {
                int index = 0;
                vector<int> blockIds;
                string studentNumber = request.data["matricule"].asString();
  
                if (strdup(request.data["sigle"].asString().c_str())[0] == '\0') {
                    classList = selectCoursesFromStudentCode(stoi(studentNumber), stoi(request.data["trimestre"].asString()));
                } else {
                    classList = selectCoursesFromStudentCode(stoi(studentNumber),
                                                        stoi(request.data["trimestre"].asString()),
                                                        strdup(request.data["sigle"].asString().c_str())
                                                        );
                }
                
                if(!classList.empty()) {
                    response["prenom"] = studentFirstName;
                    response["nom"] = studentLastName;
                    studentFirstName.erase();
                    studentLastName.erase();
                }

                for(std::list<Json::Value>::iterator it = classList.begin(); it != classList.end(); ++it){
                    int blockID = selectBlockIdFromCourses(strdup((*it)["sigle"].asString().c_str()),
                                                            stoi((*it)["trimestre"].asString())
                                                            );                                      
                    response["resultats"][index]["nom"] = (*it)["nom"];
                    response["resultats"][index]["sigle"] = (*it)["sigle"];
                    response["resultats"][index]["trimestre"] = (*it)["trimestre"];
                    response["resultats"][index]["note"] = (*it)["note"];
                    blockIds.push_back(blockID);
                    index++;
                }

                if(!response.empty()) {
                    
                    if (!blockchain.validateStudentData(blockIds, response, stoi(studentNumber))) {
                        logsManager.appendLog("ATTENTION", "L'information de la base de donnees n'est pas valide pour l'etudiant " +  
                                             studentNumber +  "!");
                        cout << "Blockchain : l'information de la base de données n'est pas valide pour l'étudiant " +  studentNumber +  "!" << endl;
                        response["resultats"] = Value();
                    } else {
                        cout << "Blockchain : Information de la base de donnée valide!" << endl;
                    }
                } else {
                    logsManager.appendLog("ERREUR", "L'information de l'etudiant " + studentNumber + " n'est pas dans la base de donnees!");
                    cout << "L'information de l'étudiant " + studentNumber + " n'est pas dans la base de données!" << endl;
                }

                break;
            }
        case GET_BLOCKCHAIN_FILE:
            {
                string lastBlock = request.data["dernier_blocs"].asString();
                response = blockchain.sendBlockchain(stoi(lastBlock));
                logsManager.appendLog("INFO", "Chaine de blocs envoyee a partir du bloc " + lastBlock + "!");
                cout << "Chaîne de blocs envoyée a partir du bloc " + lastBlock + "!" << endl;
                break;
            }
        case GET_MINER_LOGS:
            {
                response = logsManager.sendLogs(stoi(request.data["dernier_log"].asString()));
                break;
            }
        case GET_PDF:
            {
                response["pdf"] = selectPdfFromCourse(strdup(request.data["sigle"].asString().c_str()), 
                                                    stoi(request.data["trimestre"].asString()));
                break;
            }
        default:
            cout << "Type de requête invalide";
    }
    return response;
}

#endif