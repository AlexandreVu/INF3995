#include <map>
#include <list>
#include <iostream>
#include <json/json.h>
#include "jwt/jwt_all.h"
#include "DatabaseManager.cpp"
#include "UtilityClass.cpp"

void login_handler(const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string username = obj["usager"].asString();
        string password = obj["mot_de_passe"].asString();

        if (username == "" || password == "") {
            logsManager.appendLog("ERREUR", "Format de la requete de connexion invalide");
            session->close(BAD_REQUEST, "Bad request", {{"Content-Length", "15"}, {"Connection", "close"}});
            return;
        }
        
        if ( session->get_request()->get_path() == "/admin/login" && username != "admin") {
            logsManager.appendLog("ATTENTION", "Connexion au compte Usager : " + username + " non autorise");
            session->close(UNAUTHORIZED, "Not an admin account", {{"Connection", "Close"}});
            return;
        }

        if ( session->get_request()->get_path() == "/usager/login" && username == "admin") {
            logsManager.appendLog("ATTENTION", "Connexion au compte Admin : " + username + " non autorise");
            session->close(UNAUTHORIZED, "Not a user account", {{"Connection", "Close"}});
            return;
        }

        cout << "Utilisateur " + username + " tente de s'authentifier." << endl;

        User user;
        list<User> users = selectData(ANDROID_DB, username);

        if (users.empty()) {
            logsManager.appendLog("ATTENTION", "Utilisateur : " + username + " non trouve");
            session->close(FORBIDDEN, "User not found!", {{"Content-Length", "15"}});
            return;
        } else {
            user = users.front();
            string hashedPassword = createHashSha256(password);
            if (hashedPassword != user.password)
            {
                logsManager.appendLog("ATTENTION", "Mot de passe invalide pour l'utilisateur : " + username);
                session->close(FORBIDDEN, "User not found!", {{"Content-Length", "15"}});
                return;
            }
        }

        cout << "Utilisateur " + username + " authentifié !" << endl;

        string tokenId = string(user.id) + getRandomString(10);

        for (auto it = loggedUsers.begin(); it != loggedUsers.end(); it++ )
        {
            if(it->first == user.username){
                cout << "Utilisateur " + username + " déjà connecté!" << endl;
                logsManager.appendLog("ATTENTION", "Utilisateur : " + user.username + " deja connecte");
                session->close(FORBIDDEN, "User already connected!", {{"Content-Length", "23"}});
            }
        }

        loggedUsers.insert(pair<string, string>(user.username, tokenId));

        json tokenPayload = {{"iss", user.username},
                             {"jti", tokenId},
                             {"edition", user.isEditionMode ? "true" : "false"},
                             {"exp", 1613595793}};

        string token = generateToken(SECRET_KEY, tokenPayload);

        Json::Value loginValue;
        loginValue["token"] = token;
        loginValue["edition"] = user.isEditionMode ? "true" : "false";
        string loginSend = fastWriter.write(loginValue);

        logsManager.appendLog("INFO", "Connexion autorise pour l'utilisateur : " + user.username);
        session->close(OK, loginSend, {{"Connection", "close"}});
    });
}

void logout_handler(const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string jsonWebToken = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jsonWebToken);
        sleep(0.05);

        if (!assertValidTokenId(payload["jti"], payload["iss"]))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        cout << "Utilisateur " << payload["iss"] << " s'est déconnecté!" << endl;

        string username = payload["iss"];
        loggedUsers.erase(payload["iss"]);
        logsManager.appendLog("INFO", "Deconnexion du compte : " + username);
        session->close(OK, {{"Connection", "close"}});
    });
}

void change_password_handler(const shared_ptr<Session> session)
{
    const auto request = session->get_request();
    int content_length = request->get_header("Content-Length", 0);

    session->fetch(content_length, [](const shared_ptr<Session> session, const Bytes &body) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(string(body.begin(), body.end()), obj);

        string oldPassword = obj["ancien"].asString();
        string newPassword = obj["nouveau"].asString();

        if (oldPassword == "" || newPassword == "")
        {
            logsManager.appendLog("ERREUR", "Format du changement de mot de passe invalide");
            session->close(BAD_REQUEST, {{"Connection", "close"}});
            return;
        }

        string jsonWebToken = obj["jwt"].asString();
        json header, payload;
        tie(header, payload) = decodeToken(jsonWebToken);
        string username = payload["iss"];

        sleep(0.05);
        if (!assertValidTokenId(payload["jti"], username))
        {
            logsManager.appendLog("ERREUR", "Utilisateur non autorise");
            session->close(UNAUTHORIZED, {{"Connection", "close"}});
            return;
        }

        cout << "Changement de mot de passe accepté" << endl;

        User user;
        list<User> users = selectData(ANDROID_DB, username);
        user = users.front();

        if (createHashSha256(oldPassword) != user.password) {
            string msg = "Old password does not match!";
            logsManager.appendLog("ATTENTION", "L'ancien mot de passe rentre ne correspond pas a votre ancien mot de passe");
            session->close(BAD_REQUEST, msg, {{"Content-Length", to_string(msg.length())}});
            return;
        }

        updateData(ANDROID_DB, username, newPassword);

        logsManager.appendLog("INFO", "L'utilisateur : " + username + " a bien change de mot de passe");
        session->close(OK, {{"Connection", "close"}});
    });
}