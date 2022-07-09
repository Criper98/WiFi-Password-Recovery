// File per le funzioni

void PrintTitle(string Version, short Returns=0)
{
	TextColor tc;
	tc.SetColor(tc.Lime);
	
	cout<<"  __        ___ _____ _   ____                                     _   ____                                    "<<endl;
	cout<<"  \\ \\      / (_)  ___(_) |  _ \\ __ _ ___ _____      _____  _ __ __| | |  _ \\ ___  ___ _____   _____ _ __ _   _ "<<endl;
	cout<<"   \\ \\ /\\ / /| | |_  | | | |_) / _` / __/ __\\ \\ /\\ / / _ \\| '__/ _` | | |_) / _ \\/ __/ _ \\ \\ / / _ \\ '__| | | |"<<endl;
	cout<<"    \\ V  V / | |  _| | | |  __/ (_| \\__ \\__ \\\\ V  V / (_) | | | (_| | |  _ <  __/ (_| (_) \\ V /  __/ |  | |_| |"<<endl;
	cout<<"     \\_/\\_/  |_|_|   |_| |_|   \\__,_|___/___/ \\_/\\_/ \\___/|_|  \\__,_| |_| \\_\\___|\\___\\___/ \\_/ \\___|_|   \\__, |"<<endl;
	cout<<"| By Criper98                                                                                            |___/ "<<endl;
	cout<<"| Versione ["<<Version<<"]"<<endl;
	tc.SetColor(tc.Default);
	for(int i=0; i<Returns; i++)
		cout<<endl;
	return;
}

string GetChatID(TelegramBot& tb, Log& lg) // Funzione per ottenere l'ID della chat al quale il bot Telegram dovrà mandare i messaggi
{
	LastMsg lm; // Struttura definita in EssCurl per salvare i dati dell'ultimo messaggio ricevuto dal bot
	GeneralUtils gu;
	CLInterface cli;
	TextColor tc;
	LoadingBar lb;
	int Number=gu.GetRandomNumber(999999); // Genero un numero randomico tra 0 e 999998
	
	cout<<"Codice: ["; tc.SetColor(tc.Lime); cout<<Number; tc.SetColor(tc.Default); cout<<"]"<<endl;
	cout<<"Attendo ricezione codice";
	lb.DotsBar();
	
	while(true) // Loop per controllare ad intervalli di 1s i nuovi messaggi ricevuti sul bot
	{
		lm=tb.GetLastMessage(); // Interrogo le API di Telegram per ottenere l'ultimo messaggio, se non ci sono nuovi messaggi tutti gli elementi di LastMsg vengono popolati con un "-1"
		
		if(lm.ChatID.find("-1") == string::npos) // Se ricevo un messaggio:
		{
			tb.UpdateID++; // Aggiorno l'ID del messaggio per segnarlo come "già letto" internamente
			if(lm.Text == to_string(Number)) // Se il messaggio ricevuto corrisponde al numero generato:
			{
				lb.StopBar(100);
				
				cout<<"\nChat associata all'utente [";
				tc.SetColor(tc.Green);
				cout<<lm.Username;
				tc.SetColor(tc.Default);
				cout<<"]"<<endl;
				
				tb.SendMessage(lm.ChatID, "Chat associata!");
				Sleep(2000);
				return lm.ChatID;
			}
			else
				lg.WriteLog("Codice errato, codice corretto ["+to_string(Number)+"] codice ricevuto ["+lm.Text+"].");
		}
		
		Sleep(1000);
	}
	
	return "-1"; // Non succede, ma se succede...
}

bool VerificaChatID(SimpleFileSettings& sfs, Log& lg)
{
	if(sfs.GetSetting("Chat_ID") == "-1")
	{
		lg.WriteLog("[Errore]: Associazione ChatID fallita.");
		cout<<"Qualcosa e' andato storto durante l'associazione."<<endl;
		system("pause");
		return false;
	}
	
	return true;
}

VectSettings GetSettings(SimpleFileSettings& sfs, Log& lg)
{
	VectSettings Settings(3);
	
	Settings[0].Name = "Salva";
	Settings[0].Escape = true;
	
	Settings[1].Name = "Nascondi Console";
	Settings[1].CheckValue = "true";
	Settings[1].Value = sfs.GetSetting("HideConsole");
	
	Settings[2].Name = "Controlla Update";
	Settings[2].CheckValue = "true";
	Settings[2].Value = sfs.GetSetting("CheckUpdate");
	
	lg.WriteLog("Settaggi ottenuti.");
	
	return Settings;
}

bool EseguiInterfaccia(SimpleFileSettings& sfs, string Versione, TelegramBot& tb, Log& lg)
{
	lg.WriteLog("Inizializzazione CLI.");
	
	CLInterface cli;
	EasyMenu em; // Classe per semplificare la gestione di menu CLI
	TextColor tc;
	VectString MainMenu;
	VectSettings Settings;
	
	// Inizializzo i vari menu per la CLI
	MainMenu.push_back("Esci");
	MainMenu.push_back("Cambia ChatID Associata");
	MainMenu.push_back("Impostazioni");
	MainMenu.push_back("Recupera Password");
	
	while(true)
	{
		system("cls");
		PrintTitle(Versione, 1);
		cli.SubTitle("Menu Principale", 75, tc.Green);
		
		switch(em.SingleSelMenu(MainMenu, em.Quadre))
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
				
				cout<<"Per associare la chat Telegram scrivere al bot @WiFiPRbot il seguente codice."<<endl;
				sfs.SetSetting("Chat_ID", GetChatID(tb, lg));
				if(!VerificaChatID(sfs, lg))
					return false;
				sfs.SetSetting("SetChatID", "false");
				
				lg.WriteLog("ChatID associato.");
			break;
			
			case 2:
				lg.WriteLog("Impostazioni.");
				
				for(bool i=true; i;)
				{
					system("cls");
					PrintTitle(Versione, 1);
					cli.SubTitle("Impostazioni", 75, tc.Green);
					
					Settings = GetSettings(sfs, lg);
					
					switch(em.SettingsMenu(Settings))
					{
						case 0:
							i = false;
						break;
						
						case 1:
							if(Settings[1].Value == "true")
								sfs.SetSetting("HideConsole", "false");
							else
								sfs.SetSetting("HideConsole", "true");
						break;
						
						case 2:
							if(Settings[2].Value == "true")
								sfs.SetSetting("CheckUpdate", "false");
							else
								sfs.SetSetting("CheckUpdate", "true");
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

void StampaConSpazi(string Stampa, int TotLenght, short Color)
{
	TextColor tc;
	
	tc.SetColor(Color);
	cout<<Stampa;
	for(int i=Stampa.size(); i<TotLenght; i++)
		cout<<" ";
	tc.SetColor(tc.Default);
}

void StampaPassword(VectString SSID, VectString Password)
{
	TextColor tc;
	int SsidLenght = 4;
	int PasswordLenght = 3;
	
	for(int i=0; i<SSID.size(); i++)
		if(SsidLenght < SSID[i].size())
			SsidLenght = SSID[i].size();
			
	for(int i=0; i<Password.size(); i++)
		if(PasswordLenght < Password[i].size())
			PasswordLenght = Password[i].size();
	
	// Prima riga +----+----+
	cout<<" +-";
	for(int i=0; i<SsidLenght; i++)
		cout<<"-";
	cout<<"-+-";
	for(int i=0; i<PasswordLenght; i++)
		cout<<"-";
	cout<<"-+"<<endl;
	
	// Seconda riga | SSID   | PWD    |
	cout<<" | ";
	StampaConSpazi("SSID", SsidLenght, tc.Yellow);
	cout<<" | ";
	StampaConSpazi("PWD", PasswordLenght, tc.Yellow);
	cout<<" |"<<endl;
	
	// Terza riga |-------+--------|
	cout<<" |-";
	for(int i=0; i<SsidLenght; i++)
		cout<<"-";
	cout<<"-+-";
	for(int i=0; i<PasswordLenght; i++)
		cout<<"-";
	cout<<"-|"<<endl;
	
	// Righe SSID e PWD
	for(int i=0; i<SSID.size(); i++)
	{
		cout<<" | ";
		StampaConSpazi(SSID[i], SsidLenght, tc.Lime);
		cout<<" | ";
		StampaConSpazi(Password[i], PasswordLenght, tc.Lime);
		cout<<" |"<<endl;
	}
	
	// Ultima Riga +----+----+
	cout<<" +-";
	for(int i=0; i<SsidLenght; i++)
		cout<<"-";
	cout<<"-+-";
	for(int i=0; i<PasswordLenght; i++)
		cout<<"-";
	cout<<"-+"<<endl;
}

void CheckUpdate(string Versione, Log& lg) // Funzione per controllare la presenza di Update
{
	GitHub gh; // Classe di EssCurl per l'uso semplificato delle API di GitHub
	EasyMSGB msgb;
	GeneralUtils gu;
	
	lg.WriteLog("Verifica Update.");
	string LastRelease=gh.GetRepoTag("criper98", "wifi-password-recovery", "UserAuth"); // Ottengo il TAG più recente
	
	if(LastRelease != Versione) // Se le versioni non coincidono:
	{
		lg.WriteLog("Update trovato, versione locale ["+Versione+"], versione attuale ["+LastRelease+"]");
		if(msgb.YesNo("La versione di \"WiFi Password Recovery\" che stai usando non e' aggiornata.\nScaricare la versione piu' recente?", msgb.Info, "Update!"))
			gu.OpenURL("https://github.com/Criper98/WiFi-Password-Recovery/releases/latest");
	}
}
