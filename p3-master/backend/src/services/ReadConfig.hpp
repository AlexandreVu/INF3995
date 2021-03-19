#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

struct Configuration{
  int portServer, portMinerlocal, portMiner2, portMiner3, minerId, synchronizationServicePort, minerNonce;
};

bool configure(string configFile, struct Configuration&);
