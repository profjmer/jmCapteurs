// Initialize WebSocket connection and event handlers
function setupWebsocket() {	
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
    ws = new WebSocket(wsUri);
		
	ws.onopen = function(e) {
	console.log("Client en ligne, requête lastDS1820");
	// requête des dernières valeurs
	ws.send("ds json\n");
	}

	// Listen for the close connection event
	ws.onclose = function(e) {
		log("Client parti: " + e.reason);
	}

	// Listen for connection errors
		ws.onerror = function(e) {
		log("Error ");
	}

	// Listen for new messages arriving at the client
	ws.onmessage = function(e) {
		log("Message Rx: " + e.data);
		
		if (typeof e.data === "string") {
		// Create a JSON object.
			var jsonObject = JSON.parse(e.data);
			switch(jsonObject.type) {
			case 'text':
				console.log("Rx Texte: " + jsonObject.content)
				break;
			
			case "DS1820":	
				// Extraire le tableau des valeurs de température
				var ts = jsonObject.ts;
				var T = jsonObject.T;
				if(ts=="" || ts == undefined)ts=new Date();

				document.getElementById("time").innerHTML = ts;
				var dsQte=T.length;
				
				switch(dsQte){
					case 4:					
						var DS1 = document.gauges.get('ds1');
						DS1.value = T[0];
		
						var DS2 = document.gauges.get('ds2');
						DS2.value = T[1];			
						
						var DS3 = document.gauges.get('ds3');
						DS3.value = T[2];
		
						var DS4 = document.gauges.get('ds4');
						DS4.value = T[3];
					break;
					case 3 :
						var tds4 = document.getElementById("ds4");
						if(tds4 != undefined)tds4.parentNode.removeChild(tds4);

						var DS1 = document.gauges.get('ds1');
						DS1.value = T[0];
		
						var DS2 = document.gauges.get('ds2');
						DS2.value = T[1];			
						
						var DS3 = document.gauges.get('ds3');
						DS3.value = T[2];
	
					break;
					case 2 :
						var tds4 = document.getElementById("ds4");
						if(tds4 != undefined)tds4.parentNode.removeChild(tds4);
						var tds3 = document.getElementById("ds3");
						if(tds3 != undefined)tds3.parentNode.removeChild(tds3);

						var DS1 = document.gauges.get('ds1');
						DS1.value = T[0];
		
						var DS2 = document.gauges.get('ds2');
						DS2.value = T[1];				
					break;
					case 1 :
					case 0 :
							var tds4 = document.getElementById("ds4");
						if(tds4 != undefined)tds4.parentNode.removeChild(tds4);
						var tds3 = document.getElementById("ds3");
						if(tds3 != undefined)tds3.parentNode.removeChild(tds3);
						var tds2 = document.getElementById("ds2");
						if(tds2 != undefined)tds2.parentNode.removeChild(tds2);
															
						var DS1 = document.gauges.get('ds1');
						DS1.value = T[0];
				
					break;
				}

			}   
		}
	}

	// Send a message on the WebSocket.
	function sendMessage(msg){
		ws.send(msg);
		log("Message transmis");
	}

	// Display logging information in the document.
	function log(s) {
		console.log(s);
	}	
}

