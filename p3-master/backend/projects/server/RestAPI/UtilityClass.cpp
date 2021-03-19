#ifndef UTILITYCLASS_CPP
#define UTILITYCLASS_CPP
#include <restbed>
#include <unistd.h>
#include "../LogsManager/LogsManager.cpp"
#include "../../../src/requestType.hpp"

using namespace std;
using namespace restbed;
using json = nlohmann::json;

LogsManager logsManager("logs.json");

static const char *SECRET_KEY = "secret";

static const char *ANDROID_DB = "android.db";

static map<string, string> loggedUsers;

static Json::FastWriter fastWriter;

static string getRandomString(size_t length)
{
    auto randomChar = []() -> char {
        const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

        const size_t maxIndex = (sizeof(charset) - 1);
        return charset[rand() % maxIndex];
    };
    string str(length, 0);
    generate_n(str.begin(), length, randomChar);
    return str;
}

static string generateToken(string key, json payload)
{
    HS256Validator signer(key);
    return JWT::Encode(signer, payload);
}

static bool assertValidTokenId(string tokenId, string username)
{
    return loggedUsers.find(username)->second == tokenId;
}

static tuple<json, json> decodeToken(string jsonWebToken)
{
    ExpValidator exp;
    HS256Validator signer(SECRET_KEY);
    return JWT::Decode(jsonWebToken, &signer, &exp);
}
#endif