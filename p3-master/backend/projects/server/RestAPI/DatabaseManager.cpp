#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <list>
#include "../../miner/hashCreation/sha256.hpp"
#include "UtilityClass.cpp"

using namespace std;
using Record = vector<string>;
using Records = vector<Record>;

struct User{
    string id;
    bool isEditionMode;
    string username;
    string password;
};

static int createDB(const char* s) {
    sqlite3* DB;
    int exit = 0;

    exit = sqlite3_open(s, &DB);

    return exit;
}

static int createTable(const char* s) {
    sqlite3* DB;

    char* sql = "CREATE TABLE IF NOT EXISTS USERS("
      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
      "EDITION            BOOLEAN, "
      "USERNAME           TEXT NOT NULL, "
      "PASSWORD           TEXT NOT NULL );";

    int exit = 0;

    try {
        exit =  sqlite3_open(s, &DB);

        char* messageError;
        exit = sqlite3_exec(DB, sql, NULL, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Erreur dans la création de la table USER." << endl;
            sqlite3_free(messageError);
        } else {
            cout << "Table USERS trouvée." << endl;
            sqlite3_close(DB);
        }
    } catch(const exception &e) {
        cerr << e.what();
    }
    return exit;
}

static int updateData(const char* s, string username, string password) {
    sqlite3* DB;
    char* messageError;

    int exit = sqlite3_open(s, &DB);
    string hashedPassword = createHashSha256(password);

    string sql("UPDATE USERS SET PASSWORD = '" + hashedPassword + "' WHERE USERNAME = '" + username +"'");

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Erreur dans la mise à jour du compte " + username + "." << endl;
        sqlite3_free(messageError);
    } else {
        cout << "Compte " + username + " mis à jour." << endl;
    }

    return exit;
}

static int insertData(const char* s, bool isEditionMode, string username, string password) {
    sqlite3* DB;
    char* messageError;

    int exit = sqlite3_open(s, &DB);
    string isEditionMode_val = isEditionMode ? "TRUE, ": "FALSE, ";

    string hashedPassword = createHashSha256(password);

    string sql = "INSERT INTO USERS (EDITION,USERNAME,PASSWORD) "\
                 "VALUES (" + isEditionMode_val + "'" + username + "', '" + hashedPassword + "' );";

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Erreur dans l'insertion du compte " + username + "." << endl;
        sqlite3_free(messageError);
    } else {
        cout << "Nouveau compte " + username + " inséré dans la base de donnée." << endl;
    }

    return exit;
}

static int selectCallBack(void *p_data, int num_fields, char **p_fields, char **p_col_names)
{
    Records* records = static_cast<Records*>(p_data);
    try {
        records->emplace_back(p_fields, p_fields + num_fields);
    }
    catch (...) {
        return 1;
    }
    return 0;
}

static list<User> selectData(const char* s, string username) {
    sqlite3* DB;

    int exit = sqlite3_open(s, &DB);

    string sql = "SELECT * FROM USERS WHERE USERNAME='" + username +"';";

    Records records;
    char *errmsg;
    int ret = sqlite3_exec(DB, sql.c_str(), selectCallBack, &records, &errmsg);
    if (ret != SQLITE_OK) {
        cerr << "Erreur lors du SELECT : [" << errmsg << "]\n";
    }
    else {
        cout << "Compte " + username + (records.size() ? " trouvé!" : " n'a pas été trouvé!") << endl;
    }

    list<User> users;

    for (auto& record : records) {
        User user;
        user.id = record[0];
        user.isEditionMode = record[1] == "1";
        user.username = record[2];
        user.password = record[3];
        users.push_back(user);
    }

    return users;
}

static int deleteData(const char* s, string username) {
    sqlite3* DB;

    int exit = sqlite3_open(s, &DB);

    string sql = "DELETE FROM USERS WHERE USERNAME = '" + username + "'";
    exit = sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);

    return exit;
}

void initializeAdmin(const char* s) {
    list<User> users = selectData(s, "admin");
    
    if (users.empty()) {
        insertData(s, true, "admin", "admin");
    }
}

void initializeDB() {
    sqlite3* DB;

    createDB(ANDROID_DB);
    createTable(ANDROID_DB);
}