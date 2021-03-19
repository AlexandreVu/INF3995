#include <string>
#include <time.h>
#include <json/json.h>
#include "jwt/jwt_all.h"
#include <iostream> 
#include <fstream>

using namespace std; 
using json = nlohmann::json;

int MAX_LOG_DISPLAY = 20;
class LogsManager
{
    private:
        string _filename;    

    public:
        ofstream logWriter;
        ifstream logReader;
        int logId;
        Json::Value information;
        vector<Json::Value> logVector;

        LogsManager(string filename);
        void appendLog(string severity, string inputData);
        Json::Value sendLogs(int lastId);
};

LogsManager::LogsManager(string filename)
{
    this->_filename = filename;
    this->logId = 0;
}

void LogsManager::appendLog(string severity, string inputData)
{
    Json::Value log;
    this->logId++;
    time_t date = time(0);
    log["no"] = this->logId;
    log["severite"] = severity;
    log["heure"] = ctime(&date);
    log["message"] = inputData;
    logVector.push_back(log);

}

Json::Value LogsManager::sendLogs(int lastId)
{
    if(lastId == 0 && logVector.size() >= MAX_LOG_DISPLAY){
        lastId = logVector.size() - MAX_LOG_DISPLAY;
    }

    logWriter.open(this->_filename);
    logWriter << "{\"information\": [" << endl;
    for(int i = lastId; i < logVector.size(); i++){
        logWriter << logVector[i];
        if(i != logVector.size()-1){
            logWriter << "," << endl;
        }
    }
    logWriter << "]}"<< endl;
    logWriter.close();

    logReader.open(this->_filename);
    if(logReader.good()){
        logReader >> information;
        logReader.close();
    }

    return information;
}
