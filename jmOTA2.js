// jmOTA2.js version 2.0 date 2021-01-03
/**
 * node jmota2            broadcaste sur le réseau du point d'accès 192.168.4.255
 * node jmota2 sta        broadcaste sur le réseau local 192.168.0.255
 * node jmota2 10.10.10.255 broadcaste sur le réseau dont l'adresse est fournie
 */

 // Fixer l'adresse de broadcast 
var broadcastIP;
if(process.argv[2] != undefined){
  if (process.argv[2] == "sta")broadcastIP="192.168.0.255"
  else broadcastIP=process.argv[2];
}
else broadcastIP="192.168.4.255";   //PA

// Normalement ces paramètres ne sont pas modifiés
// S'ils sont modifiés, il faut également les modifier dans l'ESP8266
var UDP_ota_Port = 54321;  // UDP, PC écoute sur ce port
var UDP_esp_Port = 8266;     // UDP, ESP8266 écoute sur ce port

// port du serveur http pour ota
var HTTP_ota_Port =8686;


// ne rien modifier ci-dessous
var otaIP;
/********************************************* */
// section serveur http pour jmOTA
var http = require('http');
var url = require('url');
var fs = require('fs');

http.createServer(function(req, res) {
  
  var purl = url.parse(req.url, true);
  //console.log(purl);
  //console.log(purl.pathname);
  if (purl.pathname == '/') {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end('Serveur jmOTA dans un conteneur sur PC.\nSert les fichiers du sous-dossier /out/firmware dans ce projet.'); 
  } else {
    fs.exists(__dirname +purl.pathname, function(exists) {
      if (exists) {
		    console.log("Téléchargement: "+__dirname +purl.pathname);
        var extention = purl.pathname.match(/\..+$/)[0];
        var mime = {
          '.js': 'text/javascript; charset=UTF-8',
          '.txt': 'text/plain; charset=UTF-8',
          '.html': 'text/html; charset=UTF-8',
          '.png': 'image/png',
          '.gif': 'image/gif',
		      '.bin': '',
          '.jpg': 'image/jpeg'
        }

        res.writeHead(200, {'Content-Type': mime[extention]});
        fs.readFile(__dirname + purl.pathname, function(err, data) {
          if (err) throw err;
          res.end(data);
        });

      } else {
		    res.writeHead(404, {'Content-Type': 'text/plain'});
        res.end('Fichier Introuvable');
      }
    });

  }

}).listen(HTTP_ota_Port);
console.log('Serveur jmOTA2 sur PC.\nSert les fichiers du sous-dossier /out/firmware dans ce projet.'+
"\nL'adresse de Broacast pour le téléchargement est "+broadcastIP+
"\n\n!!! Vérifier que votre PC est dans le même segment que celui du robot !!!\n"+
"\nFormats de commande jmOTA2.js\n"+
"\tnode jmota2    -> BroadcastIP devient 192.168.4.255\n"+
"\tnode jmota2    -> BroadcastIP devient 192.168.0.255\n"+
"\tnode jmota2  10.10.0.255  -> BroadcastIP devient l'adresse IP fournie\n"+
"\nDe plus, les ports suivants du PC doivent être ouverts:"+
"\n\tUDP "+UDP_esp_Port+", "+UDP_ota_Port+
"\n\tTCP "+HTTP_ota_Port+"\n"+
"\nIl est possible que votre antivirus ou votre parefeu bloque ces ports.\n"+
"\nPour télécharger une application par WiFi\nConnectez-vous au robot IP/commands.html et entrer ota update dans la zone de commande\n"+
"\nAprès le message de fin de téléchargement, il faut rafraichir la page.");
console.log();


// section communications UDP
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
var esp=0;

// erreur communication UDP
server.on('error', (err) => {
  console.log(`erreur udp:\n${err.stack}`);
  server.close();
});

// sur réception d'un message UDP
server.on('message', (msg, rinfo) => {
  var mess = msg.toString();
  if(mess == "jmOTA\n"){
    console.log(rinfo);
  }

  try {
    // Create a JSON object.
    //console.log(mess);
    var jsonObject = JSON.parse(mess);
    //console.log(jsonObject);

    switch(jsonObject.type ){
      case "ack":	// réception de l'identification d'un Objean
        esp++;
        console.log("Réponse "+String(esp));
        console.log(msg.toString());  
        otaIP = jsonObject.ota
        //console.log("Requête vers "+rinfo.address+":"+rinfo.port+" pour modifier l'ip du serveur OTA et pour demeuré éveillé.");
        break;  

      case "chip":	// réception de l'identification d'un Objean
        console.log("Bonjour "+msg.toString());  

        if(otaIP==undefined){
          msg = "jmOTA\n";  // cas du premier jmObjean apparaissant dans un réseau ou node jmOTA est déjà activé
          server.send(msg, 0, msg.length, UDP_esp_Port, jsonObject.station);
        } 
        else{
          // retourne une requête de réassignation de l'adresse du serveur ota vers ce nouveau jmObjean
          msg = "ota serveur "+ otaIP +"\n";
          server.send(msg, 0, msg.length, UDP_esp_Port, jsonObject.station);
          //console.log("Requête vers "+rinfo.address+":"+rinfo.port+" pour modifier l'ip du serveur OTA et pour demeuré éveillé.");
        }
        break;               
    }
    
  } catch (error) {
    //console.log(error);
    //console.log("pas du JSON");		// pas du JSON
    console.log(mess);   // message format string
  }
});

// Serveur UDP à l'écoute au port localPort
server.on('listening', () => {
  console.log("Requête d'identification des jmObjean actifs du réseau et d'assignation de l'adresse de ce PC pour les téléchargements par WiFi\n");
  var msg = "jmOTA\n";
  server.send(msg, 0, msg.length, UDP_esp_Port, broadcastIP);
});
server.bind(UDP_ota_Port);
