# WiFi Password Recovery (portable)

![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/Criper98/WiFi-Password-Recovery?color=10&label=Release)
[![Version](https://img.shields.io/badge/OS-Windows-orange)]()

**WiFi Password Recovery** è un software di recupero password delle reti Wi-Fi salvate su un PC Windows.  
Per rendere più agevole il procedimento viene utilizzato un **bot Telegram** che manda le password recuperate alla chat associata.  
Inoltre **è totalmente portatile**, basta mettere l'eseguibile e il file .ini in una **chiavetta USB** ed eseguire il programma direttamente da essa senza bisogno di nessuna installazione.

**Bot Telegram** -> [@WiFiPRbot](https://t.me/WiFiPRbot)

----
| Lato Windows | Lato Telegram |
|--|--|
|![image 1](https://i.ibb.co/stSwyY9/img1.png)|![image 2](https://i.ibb.co/R0myDtX/img2.png)|

----
## Primo Utilizzo & Impostazioni

Al primo avvio del programma verrà chiesto di associare la chat da usare per i messaggi mandati dal bot, per fare ciò sarà necessario seguire i seguenti punti:
- Avviare la chat su Telegram con il bot [@WiFiPRbot](https://t.me/WiFiPRbot).
- Avviare WiFi Password Recovery su un qualsiasi computer connesso alla rete.
- Scrivere al bot il codice generato dal programma.
- Se non ci sono problemi il bot ti risponderà "Chat associata!".

Se vuoi cambiare la chat associata al bot ti basterà utilizzare l'apposita funzione nel menu principale oppure puoi aprire il file ***settings.ini*** e cambiare il primo settaggio nella seguente maniera:
```
[SetChatID]=true\;
```
Una volta riavviato il programma ti verrà chiesto di associare nuovamente la chat.

Di seguito una tabella di tutti i settaggi contenuti nel file ***settings.ini***:
| Nome | Valori | Descrizione |
|--|--|--|
| SetChatID | true / false | Se impostato su "true" all'avvio del programma verrà richiesta l'associazione della chat Telegram. |
| Chat_ID | Valore numerico | Indica l'ID della chat Telegram alla quale il bot deve fare riferimento per mandare i messaggi. Questo valore viene impostato automaticamente, è sconsigliata la modifica manuale. |
| HideConsole | true / false | Se impostato su "true" il programma si nasconderà e procederà all'invio delle password su Telegram senza bisogno di alcun input. |
| DebugMode | true / false | Se impostato su "true" il programma va in modalità Debug. Si consiglia di tenerlo sempre "false" |
| CheckUpdate | true / false | Se impostato su "true" il programma verificherà la presenza di Update. |
| AutoDelete | true / false | Se impostato su "true" il programma e tutti i suoi files si elimineranno appena terminata l'esecuzione. |

----
## Prossimi Aggiornamenti

- [X] Check della connessione a internet. (1.0.2)
- [X] Controllo degli Update. (1.0.2)
- [X] Interfaccia CLI. (1.0.3)
- [X] Auto-eliminazione. (1.0.4)

----
## Ambienti Testati

| OS | Versione Software | Esito |
|--|--|--|
| Windows 11 | 1.0.4 | ✅ |
| Windows 10 | 1.0.4 | ✅ |
| Windows 7 | -- | Da testare |
