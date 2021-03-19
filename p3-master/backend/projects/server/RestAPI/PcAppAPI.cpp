#include "UtilityClass.cpp"

void admin_logs (const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        int lastLogRequest = stoi(obj["dernier"].asString());

        Json::Value data;
        data["dernier_log"] = obj["dernier"];

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        sleep(0.05);
        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }
        
        cout << "Requête d'affichage des logs acceptée" << endl;

        Json::Value response;
        Json::FastWriter logWriter;
        if ( session->get_request()->get_path() == "/admin/logs/1" ) {
            logsManager.appendLog("INFO", "Demande des logs du mineur 1");
            response = zmqBroker.requestInfo(data, GET_MINER_LOGS, 0);
            sleep(1);
        } else if ( session->get_request()->get_path() == "/admin/logs/2" ) {
            logsManager.appendLog("INFO", "Demande des logs du mineur 2");
            response = zmqBroker.requestInfo(data, GET_MINER_LOGS, 1);
            sleep(1);
        } else if ( session->get_request()->get_path() == "/admin/logs/3" ) {
            logsManager.appendLog("INFO", "Demande des logs du mineur 3");
            response = zmqBroker.requestInfo(data, GET_MINER_LOGS, 2);
            sleep(1);
        } else if ( session->get_request()->get_path() == "/admin/logs/serveurweb"){
            logsManager.appendLog("INFO", "Demande de Logs Serveur");
            response = logsManager.sendLogs(lastLogRequest);
            sleep(1);
        }

        string logs = logWriter.write(response);
        session->close(OK, logs, {{"Content-Length", to_string(logs.length())}});
    });
}

void admin_blockchain (const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        int lastBlockRequest = stoi(obj["dernier_blocs"].asString());

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        sleep(0.05);
        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }
        cout << "Requête d'affichage de la chaine de blocs acceptée" << endl;

        Json::Value data;
        data["dernier_blocs"] = obj["dernier_blocs"];

        Json::Value response;
        if ( session->get_request()->get_path() == "/admin/chaine/1" ) {
            response = zmqBroker.requestInfo(data, GET_BLOCKCHAIN_FILE, 0);
            logsManager.appendLog("INFO", "Demande de la chaine de blocs du mineur 1");
            sleep(1);
        } else if ( session->get_request()->get_path() == "/admin/chaine/2" ) {
            response = zmqBroker.requestInfo(data, GET_BLOCKCHAIN_FILE, 1);
            logsManager.appendLog("INFO", "Demande de la chaine de blocs du mineur 2");
            sleep(1);
        } else if ( session->get_request()->get_path() == "/admin/chaine/3" ) {
            response = zmqBroker.requestInfo(data, GET_BLOCKCHAIN_FILE, 2);
            logsManager.appendLog("INFO", "Demande de la chaine de blocs du mineur 3");
            sleep(1);
        }

        string sResponse = fastWriter.write(response);
        session->close(OK, sResponse, {{"Connection", "Close"}});
    });
}

void admin_suppression_compte (const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string username = obj["usager"].asString();

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        sleep(0.05);
        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }
        cout << "Requête de suppression de compte acceptée" << endl;

        list<User> users = selectData(ANDROID_DB, username);

        if (users.empty()){
            logsManager.appendLog("ATTENTION", "Utilisateur : " + username + " non trouve pour la suppression de compte");
            session->close(FORBIDDEN, "User not found!", {{"Content-Length", "15"}});
            return;
        } else {
            deleteData(ANDROID_DB, username);
        }

        logsManager.appendLog("INFO", "Suppression du compte : " + username);
        session->close(OK, {{"Connection", "close"}});
    });
}

void admin_creation_compte (const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string username = obj["usager"].asString();
        string password = obj["mot_de_passe"].asString();
        string editionString = obj["edition"].asString();

        bool edition = editionString == "true";

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        sleep(0.05);
        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }
        cout << "Requête de création de compte acceptée" << endl;

        list<User> users = selectData(ANDROID_DB, username);
        
        if (users.empty()){
            insertData(ANDROID_DB, edition, username, password);
        } else {
            User user = users.front();
            if (username == user.username)
            {
                logsManager.appendLog("ATTENTION", "Utilisateur : " + username + " deja existant pour la creation de compte");
                session->close(FORBIDDEN, "User already exists in database!", {{"Content-Length", "15"}});
                return;
            }
        }

        logsManager.appendLog("INFO", "Creation du compte : " + username);
        session->close(OK, {{"Connection", "close"}});
    });
}
