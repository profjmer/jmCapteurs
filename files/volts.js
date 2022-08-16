// Initialize WebSocket connection and event handlers
function setupWebsocket() {
	var ctx = document.getElementById('jauge-volts').getContext('2d');
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
	ws = new WebSocket(wsUri);

	var qtePoints =150; // qte de points du graphe à l"écran
		
	ws.onopen = function(e) {
		console.log("Client en ligne, requête valeur alimentation");
		// requête pour dernières valeurs
		ws.send("adc json\n");
	}

	// Listen for the close connection event
	ws.onclose = function(e) {
		log("WS hors ligne : " + e.reason);
	}

	// Listen for connection errors
	ws.onerror = function(e) {
		log("Erreur ");
	}

	// Listen for new messages arriving at the client
	ws.onmessage = function(e) {
		log("Message: " + e.data);
		
		if (typeof e.data === "string") {

			try {
				// Create a JSON object.
				var jsonObject = JSON.parse(e.data);
			
				if(jsonObject.type=="ADC"){
					
					// Extraire les valeurs des clés
					var volts = jsonObject.volts;
					var ts = jsonObject.ts; 
					if(ts=="" || ts == undefined)ts=new Date();
					document.getElementById("time").innerHTML = ts;
					var gaugeT = document.gauges.get('jauge-volts');
					gaugeT.value = volts;
				}
			} catch (error) {
				log("data pas JSON");			
			}

		}
	}
}

// Display logging information in the document.
function log(s) {
	console.log(s);
}



