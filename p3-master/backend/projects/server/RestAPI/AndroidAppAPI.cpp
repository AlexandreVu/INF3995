#include "UtilityClass.cpp"
#include "../zmqSingelton.h"

void transaction_handler(const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        if (obj["sigle"] == "" || obj["nom"] == "" || obj["trimestre"] == "" || obj["resultats"] == "") {
            logsManager.appendLog("ERREUR", "Format de la demande d'ajout d'information invalide");
            session->close(BAD_REQUEST, {{"Connection", "close"}});
            return;
        }

        string username = payload["iss"];
        list<User> users = selectData(ANDROID_DB, username);
        User user = users.front();

        if (!user.isEditionMode) {
            logsManager.appendLog("ATTENTION", "Utilisateur : " + username + " dans un mode d'edition invalide");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        cout << "Requête de transaction acceptée" << endl;

        Json::Value transaction;
        transaction["sigle"] = obj["sigle"];
        transaction["nom"] = obj["nom"];
        transaction["trimestre"] = obj["trimestre"];
        transaction["resultats"] = obj["resultats"];
        
        Json::Value data;
        data["transaction"] = transaction;
        data["pdf"] = obj["pdf"];

        string logData = fastWriter.write(transaction);
        logsManager.appendLog("INFO", "Demande d'ajout d'information effectue : " + logData);
        bool success = zmqBroker.pushBlock(data);
        sleep(1);
        success ? session->close(OK, {{"Connection", "Close"}}) :
                  session->close(UNAUTHORIZED, {{"Connection", "close"}});
    });
}

void info_cours_handler(const shared_ptr<Session> session) 
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        if (obj["sigle"] == "" || obj["trimestre"] == "") {
            logsManager.appendLog("ERREUR", "Format de la demande d'information pour un cours invalide");
            session->close(BAD_REQUEST, {{"Connection", "close"}});
            return;
        }

        cout << "Requête info cours acceptée" << endl;

        string username = payload["iss"];
        list<User> users = selectData(ANDROID_DB, username);
        User user = users.front();

        Json::Value data;
        data["sigle"] = obj["sigle"];
        data["trimestre"] = obj["trimestre"];

        Json::Value response;
        for (int i = 0; i <= 2; i++) {
            try {
                response = zmqBroker.requestInfo(data, GET_INFO_COURS, i);
                sleep(1);
                if (response["resultats"] != "") {
                    break;
                }
            } catch(int errorCode) {
                if ( i == 2 ) {
                    logsManager.appendLog("ATTENTION", "Aucune reponse pour la demande d'information pour un cours");
                    session->close(UNAUTHORIZED, "Server didn't get any response from all miners", {{"Connection", "close"}});
                    return;
                }
                continue;
            }
        }

        string sResponse = fastWriter.write(response);
        string logCourseRequest = fastWriter.write(data);
        logsManager.appendLog("INFO", "Demande d'information pour un cours effectue : " + logCourseRequest);
        session->close(OK, sResponse, {{"Connection", "Close"}});
    });
}

void info_etudiant_handler(const shared_ptr<Session> session) 
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        if (obj["sigle"] == "" || obj["trimestre"] == "" || obj["matricule"] == "") {
            logsManager.appendLog("ERREUR", "Format de la demande d'information pour un etudiant invalide");
            session->close(BAD_REQUEST, {{"Connection", "close"}});
            return;
        }

        cout << "Requête info etudiant acceptée" << endl;

        if (obj["trimestre"] == "*"){
            obj["trimestre"] = 0;
        } else {
            obj["trimestre"] = stoi(obj["trimestre"].asString());
        }

        if (obj["sigle"] == "*"){
            obj["sigle"] = "";
        }

        Json::Value data;
        data["sigle"] = obj["sigle"];
        data["trimestre"] = obj["trimestre"];
        data["matricule"] = obj["matricule"];

        Json::Value response;
        for (int i = 0; i <= 2; i++) {
            try {
                response = zmqBroker.requestInfo(data, GET_INFO_ETUDIANT, i);
                sleep(1);
                if (response["resultats"] != "") {
                    break;
                }
            } catch(int errorCode) {
                if ( i == 2 ) {
                    logsManager.appendLog("ATTENTION", "Aucune reponse pour la demande d'information pour un etudiant");
                    session->close(UNAUTHORIZED, "Server didn't get any response from all miners", {{"Connection", "close"}});
                    return;
                }
                continue;
            }
        }

        string sResponse = fastWriter.write(response);
        string logStudentRequest = fastWriter.write(data);
        logsManager.appendLog("INFO", "Demande d'information pour un etudiant effectue : " + logStudentRequest);
        session->close(OK, sResponse, {{"Connection", "Close"}});
    });
}

void fichier_notes_handler(const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string jwt = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jwt);

        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        if (obj["sigle"] == "" || obj["trimestre"] == "") {
            logsManager.appendLog("ERREUR", "Format de la demande d'information pour un pdf invalide");
            session->close(BAD_REQUEST, {{"Connection", "close"}});
            return;
        }

        cout << "Requête PDF acceptée" << endl;

        Json::Value data;
        data["sigle"] = obj["sigle"];
        data["trimestre"] = obj["trimestre"];

        Json::Value response;
        for (int i = 0; i <= 2; i++) {
            try {
                response = zmqBroker.requestInfo(data, GET_PDF, i);
            } catch(int errorCode) {
                if ( i == 2 ) {
                    logsManager.appendLog("ATTENTION", "Aucune reponse pour la demande d'information pour un pdf");
                    session->close(UNAUTHORIZED, "Server didn't get any response from all miners", {{"Connection", "close"}});
                    return;
                }
                continue;
            }
        }

        string sResponse = fastWriter.write(response);
        logsManager.appendLog("INFO", "Demande d'information pour un pdf effectue");
        session->close(OK, sResponse, {{"Connection", "Close"}});
    });
}