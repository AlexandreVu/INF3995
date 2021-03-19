#include "ReadConfig.hpp"

#define NUMBER_ENTRIES 7

static const char FORMAT_CHAR = '*';

bool configure(string configFile, struct Configuration& minerConfiguration){

    ifstream file(configFile);

    if(!file.is_open()){
        return false;
    }

    string line;
    string names[NUMBER_ENTRIES];
    int values[NUMBER_ENTRIES];
    int numberArgumentConfigured = 0;

    for(int i = 0; i < 8; i++)
        cout << FORMAT_CHAR;
    cout << " Miner configuration ";
    for(int i = 0; i < 8; i++)
        cout << FORMAT_CHAR;
    cout << endl;
    while(getline(file, line)){ 
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        if(line[0] == '#' || line.empty()){
            continue;
        }

        size_t delimiterPosition = line.find("=");
        string name = line.substr(0, delimiterPosition);
        string value = line.substr(delimiterPosition + 1);

        if(numberArgumentConfigured < NUMBER_ENTRIES){
            names[numberArgumentConfigured] = name;
            values[numberArgumentConfigured] = stoi(value);
            numberArgumentConfigured++;
        }

        cout << name << " : " << value << '\n';
    }

    for(int i = 0; i < 37; i++)
        cout << FORMAT_CHAR;
    cout << endl;

    file.close();
    
    for (int i = 0; i < NUMBER_ENTRIES; i++){
        if (names[i].compare("portServer") == 0){
            minerConfiguration.portServer = values[i];
        }
        else if (names[i].compare( "portMinerLocal") == 0){
            minerConfiguration.portMinerlocal = values[i];
        }
        else if (names[i].compare( "portMiner2") == 0){
            minerConfiguration.portMiner2 = values[i];
        }
        else if (names[i].compare( "portMiner3") == 0){
            minerConfiguration.portMiner3 = values[i];
        }
        else if (names[i].compare("minerId") == 0){
            minerConfiguration.minerId = values[i];
        }
        else if (names[i].compare("synchronizationServicePort") == 0){
            minerConfiguration.synchronizationServicePort = values[i];
        }
        else if (names[i].compare("minerNonce") == 0){
            minerConfiguration.minerNonce = values[i];
        }
    }
    
    return true;
}
