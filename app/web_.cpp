/**
 * @file web_.cpp
 * @author JEan Merecier profjmer@gmail.com
 * @brief serveur Web http, serveur http websocket et serveur FTP
 * @version 1.0
 */

#include "../include/application.h"

int totalActiveSockets = 0;
HttpServer server;
FTPServer ftp;

/****************************************************************/

/**
 * @brief sur demande de la page index.html
 * 
 * @param request 
 * @param response 
 */
void onIndex(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();
	response.sendTemplate(tmpl); // will be automatically deleted
}

/**
 * @brief sur demande d'un fichier par http
 * 
 * @param request 
 * @param response 
 */
void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	//Serial.println("Requête http pour fichier:"+file);
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}


/**
 * @brief Transmet un message aux fureteurs connectés.
 * 
 * @param msg 
 */
void web_wsSend(String msg)
{
	// Notify everybody about new data
	WebSocketsList &clients = server.getActiveWebSockets();
	if(clients.count()==0)return;
	for (int i = 0; i < clients.count(); i++)
		clients[i].sendString(msg);
}

/**
 * @brief pour un nouvelle connexion de fureteur
 * 
 * @param socket 
 */
void wsConnected(WebSocket& socket)
{
	totalActiveSockets++;
	//Serial.printf("Nouveau client Websocket! Total: %d\n",totalActiveSockets);
		// Notify everybody about new connection
//	WebSocketsList &clients = server.getActiveWebSockets();
//	for (int i = 0; i < clients.count(); i++)
//		clients[i].sendString("New friend arrived! Total: " + String(totalActiveSockets));
}

/**
 * @brief retourne le nombre de fureteurs connectés
 * 
 * @return uint8_t 
 */
uint8_t web_getActiveClients(){
	return totalActiveSockets;
}

/**
 * @brief réception des requêtes d'un fureteur
 * 
 * @param socket 
 * @param message 
 */
void wsMessageReceived(WebSocket& socket, const String& message)
{	
	String msg = message;
	//Serial.println("Web_Rx:"+msg);	

	// Notify everybody about new data					?????????????????????????????????????????
	WebSocketsList &clients = server.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
		clients[i].sendString(routeur(msg));
}

/**
 * @brief réception des messages binaires d'un fureteur
 * 
 * @param socket 
 * @param data 
 * @param size 
 */
void wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size)
{
//	Serial.printf("Websocket binary data recieved, size: %d\r\n", size);
}

/**
 * @brief déconnexion d'un fureteur
 * 
 * @param socket 
 */
void wsDisconnected(WebSocket& socket)
{
	totalActiveSockets--;
	char s[] = "Un client en moins! Total:";	
	//Serial.printf("%s %d\n",s,totalActiveSockets);	
}

/**
 * @brief lors de la demande de la page commands.html
 * 
 * @param request 
 * @param response 
 */
void onCommands(HttpRequest &request, HttpResponse &response)
{	
	TemplateFileStream *tmpl = new TemplateFileStream("commands.html");
	response.sendTemplate(tmpl); // will be automatically deleted
}

/**
 * @brief lors de la demande de la page volts.html
 * 
 * @param request 
 * @param response 
 */
void onVolts(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("volts.html");
	response.sendTemplate(tmpl); // will be automatically deleted
}

/**
 * @brief initialisation des serveur http websocket et ftp
 * 
 */
void web_ini()
{	//Serial.println("web_ini()");
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/commands", onCommands);
	server.addPath("/volts", onVolts);
	server.setDefaultHandler(onFile);

	// Web Sockets configuration
	server.enableWebSockets(true);
	server.setWebSocketConnectionHandler(wsConnected);
	server.setWebSocketMessageHandler(wsMessageReceived);
//	server.setWebSocketBinaryHandler(wsBinaryReceived);
	server.setWebSocketDisconnectionHandler(wsDisconnected);

	web_ftp_ini();
}

/**
 * @brief initialisation du serveur FTP
 * 
 */
void web_ftp_ini()
{	//Serial.println("ftp_ini()");
	if (!fileExist("index.html"))
		fileSetContent("index.html", "<h3>Erreur, Pas de fichier Web, Utiliser FTP pour télécharger les fichiers</h3>");

	// Start FTP server
	ftp.listen(21);
	ftp.addUser("jmer", "123"); // FTP account
}