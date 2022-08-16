function startWebsocket() {
  var wsUri = "ws://" + location.host + "/";
  ws = new WebSocket(wsUri);
 
  ws.onopen = function(evt) { 
    console.log('station ssid'); 
    ws.send("station ssid\n");
  };
  ws.onclose = function(evt) {
    document.getElementById("info").innerHTML="Socket non connecté";	
    console.log('websock close'); 
  };
  ws.onerror = function(evt) { console.log(evt); };
  ws.onmessage = function(evt) {
    console.log("Message Rx: " + evt.data);
		
	if (typeof evt.data === "string") {
      try {
        // essai de créer un objet JSON 
        var json = JSON.parse(evt.data);
        switch(json.type) { 
          case "reseau": document.getElementById("info").innerHTML= evt.data;break;    
          case "reseaux": 
								var info = document.getElementById("info");
								if(json.qte == 0) {													 
										 info.color = "red";
										 info.innerHTML= 'Aucun Réseau WiFi  Trouvé !';
										 break;
								}
								else {
									info.style.color="white";
									document.getElementById("info").innerHTML= 'Quantité trouvée: '+json.qte;
								}	
							
								// crée la liste ssid si elle n'existe pas
								if(document.getElementById("list-ssid")==undefined){
									var div = document.createElement("div");
									div.innerHTML='<div class = "list-group" id="list-ssid"> </div>';
									document.getElementById("ssid").appendChild(div);
								}
								
								// ajoute les réseaux trouvés dans la liste
								for(i=0;i<json.qte; i++){										
									var nli =document.createElement("ul");
									nli.innerHTML += infoSSID(json,i);		// choix du sprite et du style de la liste				

									document.getElementById("list-ssid").appendChild(nli);
								};
								break;          
        }   
      }
      catch( e ) {
        // pas du json  
        // affiche texte dans la zone info
        var ligne = evt.data;
      }		
	  }    	
  };	
};

function SSID(){
	var info = document.getElementById("info");
	info.innerHTML="Requête des SSID";

	// efface la liste existante
	var post = document.getElementById( "list-ssid" );
	post.parentNode.removeChild(post);
	
	// nouvelle requête websocket
	ws.send("station ssid\n");
}

// Retrouve le sprite associé à la valeur du signal
// et crée un item de la liste ssid, la couleur de backgroud du réseau actuel actif est différent des autres
function infoSSID(json, i){
	var sListe = '<li class="list-group-item" '+		// nouvel item 
		'id = " '+ json.liste[i].titre+ ' " '+		
		'onclick = "joindre(this);" '+	// événement de clic sur l'item		// identification de l'item
		'style = "background-color:';									// couleur de fonc
	if(json.actuel == json.liste[i].titre && json.actif==true)sListe += '#722">';
	else sListe += 'black">';

  var cl =		sListe +
	  '<div class="list-group-item-text" > '+
				'<div class="wifi '+GetNetworkSignalClass(json.liste[i].rssi )+'"></div>' +	// sprite du signal
				'<h4 class="list-group-item-heading">' + json.liste[i].titre + '</h4>' +				// titre
				'<div id = "encodage">'+json.liste[i].encodage+'</div>'+
				'<div id ="rssi">'+json.liste[i].rssi +" dBm</div>"+ // Encription et rsssi
		  '</div>'+
  '</li>';
return cl;
};

// retourne le style claa wifi selon le signal
function GetNetworkSignalClass(signal){
  if(signal  >= -100 && signal <= -80){
	  return 'wifi-1';
  }else if(signal > -80 && signal <= -65 ){
	  return 'wifi-2';
  }else if (signal > -65 && signal < -50){
	  return 'wifi-3';
  }else{
	  return 'wifi-4';
  }
};

// item est un object item de la liste des réseaux
function joindre(item){
	var info = document.getElementById("info");
	info.innerHTML= 'Réseau '+item.id;
	info.style.color = "yellow";

	var el = document.getElementById(item.id);
	if(el.innerText.includes("OPEN")) ws.send("station joindre" + item.id+ "\n");
	else{ joindrePW(el);	}

};

// ajoute zone d'édition et bouton à l'item, retire l'event onclick de l'item, ajoute onclick du bouton
function joindrePW(ob){
	var inp =document.createElement("input");
	inp.setAttribute("id","password");
	inp.setAttribute("type","password");
	document.getElementById(ob.id).appendChild(inp);
	
	var bt =document.createElement("button");
	bt.setAttribute("id","btsm");
	bt.setAttribute("onclick","sousmettre(this);");
	bt.setAttribute("class","btn btn-primary");
	bt.innerHTML="Sousmettre";
	document.getElementById(ob.id).appendChild(bt);
	// ajoute onclick du bouton
	
	
	// remove onclick de l'item
	var els =ob.parentNode.innerHTML;
	var i1 = els.indexOf("onclick");
	var i2 = els.indexOf("style=");
	var s1 =els.slice(0,i1);
	var s2=els.slice(i2);
	var s3 = s1+s2;
	ob.parentNode.innerHTML =s3;

//console.log("station joindre "+ssid+" "+password+"\n");
};

  // transmet la requête ajoute meaase d'information
  function sousmettre(ob){
  var t = document.getElementById("password");
  var req = "station joindre"+ ob.parentNode.id;
  console.log(req);
  
  ws.send(req+ t.value+ "\n");
  
  // ajouter message dans zone info
	var inf= document.getElementById("info")
	inf.innerHTML = "Requête : "+req;
	
  // effacer zone édition et bouton
  // cela sera fait lors de la réception de la réponse.
  // jmObjean retourne la liste ssid la page se met à jour.

 };



