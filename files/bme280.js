// Initialize WebSocket connection and event handlers
function setupWebsocket() {	
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
    ws = new WebSocket(wsUri);
		
	ws.onopen = function(e) {
	console.log("Client en ligne,requête bme json");
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
		if (typeof e.data === "string") {
			try {
				// Create a JSON object.
				var jsonObject = JSON.parse(e.data);
				switch(jsonObject.type ){
					case "BME280":	
						log(e.data);
						// Extract the values for each key.
						var temp = jsonObject.Temperature;
						var humidity = jsonObject.Humidity;
						var pression = jsonObject.Pression;
						var ts = jsonObject.ts;
						if(ts=="" || ts == undefined)ts=new Date();

						document.getElementById("time").innerHTML = ts;
						document.getElementById("temperature").innerHTML = temp;
						document.getElementById("humidity").innerHTML = humidity;
						document.getElementById("pression").innerHTML = pression;						
						break;
				}
				
			} catch (error) {
				log("data pas JSON");		
			}
		}
		else log("Erreur conversion json");
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

