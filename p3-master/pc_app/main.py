#!/usr/bin/python3
import tkinter as tk
import time
import json
import requests


TITLE_FONT = ("Helvetica", 18, "bold")
GREEN_COLOR = "#8cc83c"
BLUE_COLOR = "#41aae6"
GREY_COLOR = "#a6a8ab"
WHITE_COLOR = "#ffffff"
API_ENDPOINT = "https://b9b24597.ngrok.io"
BACK_IMAGE_PATH = "./image/back.png"
json_web_token = ""


class AdminApp(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}
        for page, geometry in zip(
            (
                LoginPage,
                MainPage,
                PasswordManagerPage,
                LogsManagerPage,
                AccountManagerPage,
                BlockchainViewerPage,
            ),
            ("700x600", "700x600", "700x600", "700x600", "700x600", "700x600"),
        ):
            page_name = page.__name__
            frame = page(parent=container, controller=self)
            self.frames[page_name] = (frame, geometry)

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame("LoginPage")

    def show_frame(self, page_name):
        frame, geometry = self.frames[page_name]
        self.update_idletasks()
        self.geometry(geometry)
        frame.tkraise()


class LoginPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.username_verify = tk.StringVar()
        self.password_verify = tk.StringVar()
        self.entry_username = tk.Entry(self, textvariable=self.username_verify)
        self.entry_password = tk.Entry(
            self, textvariable=self.password_verify, show="*"
        )

        label = tk.Label(
            self,
            text="Bienvenue sur l'application de l'administrateur",
            fg=WHITE_COLOR,
            bg=BLUE_COLOR,
            font=TITLE_FONT,
            bd=10,
        )
        label.pack(side="top", fill="x", pady=(0,20))

        tk.Label(self, text="Nom d'usager *").pack()
        self.entry_username.pack(pady=(0,20))

        tk.Label(self, text="Mot de passe * ").pack()
        self.entry_password.pack(pady=(0,20))

        tk.Button(
            self,
            text="Se Connecter",
            width=23,
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            command=self.login_verify,
        ).pack(pady=(0,20))

    def login_verify(self):
        data = {
            "usager": self.username_verify.get(),
            "mot_de_passe": self.password_verify.get(),
        }
        global json_web_token

        response = requests.post(url=API_ENDPOINT + "/admin/login", json=data)
        message = ""
        self.entry_username.delete(0, tk.END)
        self.entry_password.delete(0, tk.END)

        if response.ok:
            json_web_token = str(response.json()["token"])
            self.controller.show_frame("MainPage")
        elif response.status_code // 100 == 5:
            message = "Une erreur est arrivée avec le serveur."
        elif response.status_code // 100 == 4:
            message = "Le mot de passe et le nom d'utilisateur ne correspondent pas à l'admin."

        label = tk.Label(self, text=message)
        label.pack()
        label.after(2000, label.destroy)


class MainPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller

        label = tk.Label(
            self,
            text="Menu Principal",
            fg=WHITE_COLOR,
            bg=BLUE_COLOR,
            font=TITLE_FONT,
            bd=10,
        )
        label.pack(side="top", fill="x", pady=(0,20))

        tk.Button(
            self,
            text="Chaine de Blocs",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            command=self.consult_blockchain,
            width=23,
        ).pack(pady=(0,20))

        tk.Button(
            self,
            text="Gestionnaire de comptes",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            command=self.create_account,
            width=23,
        ).pack(pady=(0,20))

        tk.Button(
            self,
            text="Gestionnaire des logs",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            command=self.consult_logs,
            width=23,
        ).pack(pady=(0,20))

        tk.Button(
            self,
            text="Gestionnaire de mot de passe",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            command=self.change_password,
            width=23,
        ).pack(pady=(0,20))

        tk.Button(
            self,
            text="Se Déconnecter",
            command=self.logout,
            bg=GREY_COLOR,
            fg=WHITE_COLOR,
            width=23,
        ).pack(pady=(0,20))

    def logout(self):
        global json_web_token
        token = {"jwt": json_web_token}
        response = requests.post(url=API_ENDPOINT + "/admin/logout", json=token)
        tk.Label(self, text=response.text).pack()

        message = ""

        if response.ok:
            self.controller.show_frame("LoginPage")
        elif response.status_code // 100 == 5:
            message = "Une erreur est arrivée avec le serveur."
        elif response.status_code // 100 == 4:
            message = "Une erreur est arrivée avec le client."

        label = tk.Label(self, text=message)
        label.pack()
        label.after(1000, label.destroy)

    def change_password(self):
        self.controller.show_frame("PasswordManagerPage")

    def consult_logs(self):
        self.controller.show_frame("LogsManagerPage")

    def create_account(self):
        self.controller.show_frame("AccountManagerPage")

    def consult_blockchain(self):
        self.controller.show_frame("BlockchainViewerPage")


class PasswordManagerPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.old_password = tk.StringVar()
        self.new_password = tk.StringVar()
        self.new_password_confirmation = tk.StringVar()
        self.entry_old_password = tk.Entry(
            self, textvariable=self.old_password, show="*"
        )
        self.entry_new_password = tk.Entry(
            self, textvariable=self.new_password, show="*"
        )
        self.entry_new_password_confirmation = tk.Entry(
            self, textvariable=self.new_password_confirmation, show="*"
        )
        self.back_image = tk.PhotoImage(file=BACK_IMAGE_PATH)

        label = tk.Label(
            self,
            text="Gestionnaire de mot de passe",
            fg=WHITE_COLOR,
            bg=BLUE_COLOR,
            font=TITLE_FONT,
            bd=10,
        )
        label.pack(side="top", fill="x", pady=(0,20))
        tk.Button(
            self,
            image=self.back_image,
            bg=GREEN_COLOR,
            command=self.go_back,
            compound=tk.LEFT,
            width=40,
        ).place(x=10, y=9)

        tk.Label(self, text="Ancien mot de passe *").pack()
        self.entry_old_password.pack(pady=(0,20))

        tk.Label(self, text="Nouveau mot de passe * ").pack()
        self.entry_new_password.pack(pady=(0,20))

        tk.Label(self, text="Confirmez le nouveau mot de passe * ").pack()
        self.entry_new_password_confirmation.pack(pady=(0,20))

        tk.Button(
            self,
            text="Changer de mot de passe",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            width=23,
            command=self.change_password,
        ).pack(pady=(0,20))

    def change_password(self):
        if self.new_password.get() != self.new_password_confirmation.get():
            label = tk.Label(
                self,
                text="Le nouveau mot de passe et la confirmation de ce dernier ne correspondent pas",
            )
            label.pack()
            label.after(2000, label.destroy)

        else:
            global json_web_token

            data = {
                "jwt": json_web_token,
                "ancien": self.old_password.get(),
                "nouveau": self.new_password.get(),
            }

            response = requests.post(url=API_ENDPOINT + "/admin/motdepasse", json=data)
            message = ""
            self.entry_old_password.delete(0, tk.END)
            self.entry_new_password.delete(0, tk.END)
            self.entry_new_password_confirmation.delete(0, tk.END)

            if response.ok:
                message = "Mot de passe changé avec succès!"
            elif response.status_code // 100 == 5:
                message = "Une erreur est arrivée avec le serveur."
            elif response.status_code // 100 == 4:
                message = "Le changement de mot de passe a échoué."

            label = tk.Label(self, text=message)
            label.pack()
            label.after(2000, label.destroy)

    def go_back(self):
        self.controller.show_frame("MainPage")


class LogsManagerPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.is_open = False
        self.logs_window = None
        self.back_image = tk.PhotoImage(file=BACK_IMAGE_PATH)

        self.log_index = tk.StringVar()
        self.logs_system_option = tk.StringVar()
        self.logs_system_option.set("Serveur")

        self.entry_log_index = tk.Entry(self, textvariable=self.log_index)
        logs_options_tuple = ("Serveur", "Mineur 1", "Mineur 2", "Mineur 3")
        self.logs_options = tk.OptionMenu(
            self, self.logs_system_option, *logs_options_tuple
        )
        self.logs_options.config(width=15)

        label = tk.Label(
            self,
            text="Gestionnaire des logs",
            fg=WHITE_COLOR,
            bg=BLUE_COLOR,
            font=TITLE_FONT,
            bd=10,
        )
        label.pack(side="top", fill="x", pady=(0,20))

        tk.Button(
            self,
            image=self.back_image,
            bg=GREEN_COLOR,
            command=self.go_back,
            compound=tk.LEFT,
            width=40,
        ).place(x=10, y=9)

        tk.Label(self, text="Choisissez le système voulu pour les logs").pack()
        self.logs_options.pack(pady=(0,20))

        tk.Label(self, text="Entrez le dernier LogID reçu").pack()
        self.entry_log_index.pack(pady=(0,20))

        tk.Button(
            self,
            text="Ouvrir Logs",
            bg=GREEN_COLOR,
            command=self.show_logs,
            fg=WHITE_COLOR,
            width=23,
        ).pack(pady=(0,20))

    def show_logs(self):

        if self.is_open:
            label = tk.Label(self, text="Fenêtre des logs déjà ouverte!")
            label.pack()
            label.after(1000, label.destroy)
        else:
            global json_web_token

            data = {"jwt": json_web_token, "dernier": self.log_index.get()}
            option_chosen = self.logs_system_option.get()

            if option_chosen == "Serveur":
                response = requests.post(
                    url=API_ENDPOINT + "/admin/logs/serveurweb", json=data
                )

            else:
                response = requests.post(
                    url=API_ENDPOINT + "/admin/logs/" + option_chosen.split()[1],
                    json=data,
                )

            message = ""
            self.entry_log_index.delete(0, tk.END)

            if response.ok:
                self.logs_window = tk.Tk()
                self.logs_window.title("Logs du " + option_chosen)
                self.logs_window.geometry("700x600")
                self.logs_window.protocol("WM_DELETE_WINDOW", self.on_close_logs_window)
                self.is_open = True

                scrollbar = tk.Scrollbar(self.logs_window)
                scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

                listbox = tk.Listbox(self.logs_window, height=200, width=400)
                listbox.pack()

                for log in response.json()["information"]:
                    listbox.insert(tk.END, "{} {} {}".format(log["no"], log["severite"], log["heure"][:-1]))
                    if "Demande d'ajout d'information effectue :" in log["message"]:
                        listbox.insert(tk.END, "Demande d'ajout d'information effectue :")
                        data = json.loads(log["message"].split(": ")[1].strip())
                        listbox.insert(tk.END, "    Nom: {}".format(data["nom"]))
                        listbox.insert(tk.END, "    Sigle: {}".format(data["sigle"]))
                        listbox.insert(tk.END, "    Trimestre: {}".format(data["trimestre"]))
                        listbox.insert(tk.END, "    Resultats: ")
                        for results in data["resultats"]:
                            listbox.insert(
                                tk.END,
                                "           Matricule: {} | Nom: {} | Prénom: {} | Note: {}.".format(
                                    results["matricule"],
                                    results["nom"],
                                    results["prenom"],
                                    results["note"],
                                ),
                            )

                    elif "Demande d'information pour un cours effectue :" in log["message"]:
                        listbox.insert(tk.END, "Demande d'information pour un cours effectue :")
                        data = json.loads(log["message"].split(": ")[1].strip())
                        listbox.insert(tk.END, "    Sigle: {}".format(data["sigle"]))
                        listbox.insert(tk.END, "    Trimestre: {}".format(data["trimestre"]))

                    elif "Demande d'information pour un etudiant effectue :" in log["message"]:
                        listbox.insert(tk.END, "Demande d'information pour un etudiant effectue :")
                        data = json.loads(log["message"].split(": ")[1].strip())
                        listbox.insert(tk.END, "    Matricule: {}".format(data["matricule"]))
                        listbox.insert(tk.END, "    Sigle: {}".format(data["sigle"]))
                        listbox.insert(tk.END, "    Trimestre: {}".format(data["trimestre"]))

                    else:
                        listbox.insert(tk.END, log["message"])

                    listbox.insert(
                        tk.END, "_" * 97,
                    )
                    listbox.insert(tk.END, "")

                listbox.config(
                    yscrollcommand=scrollbar.set, xscrollcommand=scrollbar.set
                )
                scrollbar.config(command=listbox.yview)

            elif response.status_code // 100 == 5:
                message = "Une erreur est arrivée avec le serveur."
            elif response.status_code // 100 == 4:
                message = "Informations entrées invalides!"

            label = tk.Label(self, text=message)
            label.pack()
            label.after(2000, label.destroy)

    def on_close_logs_window(self):
        self.is_open = not self.is_open
        self.logs_window.destroy()

    def go_back(self):
        self.controller.show_frame("MainPage")


class AccountManagerPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.back_image = tk.PhotoImage(file=BACK_IMAGE_PATH)

        self.username = tk.StringVar()
        self.password = tk.StringVar()
        self.edition = tk.IntVar()
        self.username_to_delete = tk.StringVar()

        self.entry_username = tk.Entry(self, textvariable=self.username)
        self.entry_password = tk.Entry(self, textvariable=self.password, show="*")
        self.radiobutton_edition = tk.Radiobutton(
            self, text="Edition", variable=self.edition, value=1
        )
        self.radiobutton_consultation = tk.Radiobutton(
            self, text="Consultation", variable=self.edition, value=0
        )
        self.entry_username_to_delete = tk.Entry(
            self, textvariable=self.username_to_delete
        )

        label = tk.Label(
            self,
            text="Gestionnaire de comptes",
            fg=WHITE_COLOR,
            bg=BLUE_COLOR,
            font=TITLE_FONT,
            bd=10,
        )
        label.pack(side="top", fill="x", pady=(0,10))
        tk.Button(
            self,
            image=self.back_image,
            bg=GREEN_COLOR,
            command=self.go_back,
            compound=tk.LEFT,
            width=40,
        ).place(x=10, y=9)

        label = tk.Label(self, text="Création d'un compte", font=TITLE_FONT,)
        label.pack(side="top", fill="x", pady=(0,10))

        tk.Label(self, text="Entrez un nom d'utilisateur").pack()
        self.entry_username.pack(pady=(0,10))

        tk.Label(self, text="Entrez un mot de passe").pack()
        self.entry_password.pack(pady=(0,10))

        tk.Label(self, text="Accès du compte:",).pack()
        self.radiobutton_edition.pack()
        self.radiobutton_consultation.pack(pady=(0,10))

        tk.Button(
            self,
            text="Créer un nouveau compte",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            width=23,
            command=self.create_account,
        ).pack(pady=(0,10))

        tk.Label(self, text="_" * 54).pack(pady=(0,20))

        label = tk.Label(self, text="Suppression d'un compte", font=TITLE_FONT)
        label.pack(side="top", fill="x", pady=(0,10))

        tk.Label(self, text="Entrez l'usager que vous voulez supprimer").pack()
        self.entry_username_to_delete.pack(pady=(0,10))

        tk.Button(
            self,
            text="Supprimer un compte",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            width=23,
            command=self.delete_account,
        ).pack(pady=(0,10))

    def create_account(self):
        global json_web_token

        if len(self.username.get()) < 4 or len(self.password.get()) < 4:
            message = "Nom d'utilisateur ou mot de passe trop court. Réessayez!"
            
        else:
            account = {
                "jwt": json_web_token,
                "usager": self.username.get(),
                "mot_de_passe": self.password.get(),
                "edition": str(bool(self.edition.get())).lower(),
            }

            response = requests.post(
                url=API_ENDPOINT + "/admin/creationcompte", json=account
            )
            message = ""

            self.entry_username.delete(0, tk.END)
            self.entry_password.delete(0, tk.END)

            if response.ok:
                label = tk.Label(self, text="Compte créé avec succes!")
                label.pack()
                label.after(2000, label.destroy)
            elif response.status_code // 100 == 5:
                message = response.text
            elif response.status_code // 100 == 4:
                message = "Une erreur est arrivée lors de la création du compte. Réessayez!"

        label = tk.Label(self, text=message)
        label.pack()
        label.after(2000, label.destroy)

    def delete_account(self):
        global json_web_token

        account = {"jwt": json_web_token, "usager": self.username_to_delete.get()}

        response = requests.post(
            url=API_ENDPOINT + "/admin/suppressioncompte", json=account
        )

        self.entry_username_to_delete.delete(0, tk.END)
        message = ""

        if response.ok:
            label = tk.Label(self, text="Compte supprimé avec succès!")
            label.pack()
            label.after(2000, label.destroy)
        elif response.status_code // 100 == 5:
            message = response.text
        elif response.status_code // 100 == 4:
            message = (
                "Une erreur est arrivée lors de la suppression du compte. Réessayez!"
            )

        label = tk.Label(self, text=message)
        label.pack()
        label.after(1000, label.destroy)

    def go_back(self):
        self.controller.show_frame("MainPage")


class BlockchainViewerPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.is_open = False
        self.blockchain_window = None
        self.back_image = tk.PhotoImage(file=BACK_IMAGE_PATH)
        self.block_index = tk.StringVar()
        self.miner_number = tk.StringVar()
        self.miner_number.set("Mineur 1")
        self.entry_block_index = tk.Entry(self, textvariable=self.block_index)
        miner_options_tuple = ("Mineur 1", "Mineur 2", "Mineur 3")
        self.miner_number_options = tk.OptionMenu(
            self, self.miner_number, *miner_options_tuple
        )
        self.miner_number_options.config(width=15)

        label = tk.Label(
            self,
            text="Chaîne de blocs",
            fg=WHITE_COLOR,
            bg=BLUE_COLOR,
            font=TITLE_FONT,
            bd=10,
        )
        label.pack(side="top", fill="x", pady=(0,20))

        tk.Button(
            self,
            image=self.back_image,
            bg=GREEN_COLOR,
            command=self.go_back,
            compound=tk.LEFT,
            width=40,
        ).place(x=10, y=9)

        tk.Label(self, text="Choisissez le numéro de mineur à consulter").pack()
        self.miner_number_options.pack(pady=(0,20))

        tk.Label(self, text="Entrez le dernier BlockID reçu").pack()
        self.entry_block_index.pack(pady=(0,20))

        tk.Button(
            self,
            text="Ouvir la Blockchain",
            bg=GREEN_COLOR,
            fg=WHITE_COLOR,
            width=23,
            command=self.consult_blockchain,
        ).pack(pady=(0,20))

    def consult_blockchain(self):
        if self.is_open:
            label = tk.Label(self, text="Fenêtre de la Blockchain déjà ouverte!")
            label.pack()
            label.after(1000, label.destroy)
        else:

            global json_web_token

            data = {"jwt": json_web_token, "dernier_blocs": self.block_index.get()}
            response = requests.post(
                url=API_ENDPOINT
                + "/admin/chaine/"
                + self.miner_number.get().split()[1],
                json=data,
            )

            message = ""
            self.entry_block_index.delete(0, tk.END)

            if response.ok:
                self.blockchain_window = tk.Tk()
                self.blockchain_window.title(self.miner_number.get())
                self.blockchain_window.geometry("700x600")
                self.blockchain_window.protocol(
                    "WM_DELETE_WINDOW", self.on_close_blockchain_window
                )
                self.is_open = True

                scrollbar = tk.Scrollbar(self.blockchain_window)
                scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

                scrollbar1 = tk.Scrollbar(self.blockchain_window, orient="horizontal")
                scrollbar1.pack(side=tk.BOTTOM, fill=tk.X)

                listbox = tk.Listbox(self.blockchain_window, height=100, width=200)
                listbox.pack()
                if response.json():
                    for block in response.json()["blockchain"]:
                        listbox.insert(tk.END, "Indice: {}".format(block["index"]))
                        listbox.insert(tk.END, "Nonce: {}".format(block["nonce"]))
                        listbox.insert(tk.END, "Hash: {}".format(block["hash"]))
                        listbox.insert(
                            tk.END, "Hash précedent: {}".format(block["previousHash"])
                        )
                        listbox.insert(tk.END, "Heure: {}".format(block["time"]))
                        listbox.insert(
                            tk.END, "Vérification: {}".format(block["verification"])
                        )
                        listbox.insert(tk.END, "Données: ")

                        if block["data"] != "Initial Block":
                            data = json.loads(block["data"])
                            listbox.insert(tk.END, "    Sigle: {}".format(data["sigle"]))
                            listbox.insert(
                                tk.END, "    Nom du cours: {}".format(data["nom"])
                            )
                            listbox.insert(
                                tk.END, "    Trimestre: {}".format(data["trimestre"]),
                            )
                            listbox.insert(tk.END, "    Résultats: ")
                            for results in data["resultats"]:
                                listbox.insert(
                                    tk.END,
                                    "           Matricule: {} | Nom: {} | Prénom: {} | Note: {}.".format(
                                        results["matricule"],
                                        results["nom"],
                                        results["prenom"],
                                        results["note"],
                                    ),
                                )

                        else:
                            listbox.insert(tk.END, "    {}".format(block["data"]))

                        listbox.insert(tk.END, "")
                        listbox.insert(
                            tk.END, "_" * 97,
                        )
                        listbox.insert(tk.END, "")

                    listbox.config(
                        yscrollcommand=scrollbar.set, xscrollcommand=scrollbar1.set
                    )
                    scrollbar.config(command=listbox.yview)
                    scrollbar1.config(command=listbox.xview)

            elif response.status_code // 100 == 5:
                message = "Une erreur est arrivée avec le serveur."
            elif response.status_code // 100 == 4:
                message = "Informations entrées invalides!"

            label = tk.Label(self, text=message)
            label.pack()
            label.after(2000, label.destroy)

    def on_close_blockchain_window(self):
        self.is_open = not self.is_open
        self.blockchain_window.destroy()

    def go_back(self):
        self.controller.show_frame("MainPage")


if __name__ == "__main__":
    app = AdminApp()
    app.title("Application PC de l'administrateur")
    app.mainloop()
