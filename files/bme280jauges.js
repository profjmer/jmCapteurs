// Initialize WebSocket connection and event handlers
function setupWebsocket() {	
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
  ws = new WebSocket(wsUri);
		
	ws.onopen = function(e) {
	console.log("Client en ligne, requête bme json");
	// requête pour dernières valeurs
	ws.send("bme json\n");
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
			try {
				var jsonObject = JSON.parse(e.data);
				switch(jsonObject.type) {
					case 'text':
						console.log("Rx Texte: " + jsonObject.content)
						break;
					
					case "BME280":	
						// Extraire le tableau des valeurs de température
						var ts = jsonObject.ts;
						var temperature = jsonObject.Temperature;
						var pression = jsonObject.Pression;
						var humidity = jsonObject.Humidity;

						document.getElementById("time").innerHTML = ts;

						var DS1 = document.gauges.get('bme-t');
						DS1.value = temperature;

						var DS2 = document.gauges.get('bme-p');
						DS2.value = pression;	

						var DS3 = document.gauges.get('bme-h');
						DS3.value = humidity;							

						break;			
				}  
				
			} catch (error) {
				log("data pas JSON");		
			}
		}
		else log("data pas une String");
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


