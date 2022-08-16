// Initialize WebSocket connection and event handlers
function setupWebsocket() {	
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
    ws = new WebSocket(wsUri);
		
	ws.onopen = function(e) {
	console.log("Client en ligne,requête lastDHT22");
	// requête pour dernières valeurs
	ws.send("dht json\n");
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

				case "DHT22":	
				// Extract the values for each key.
				var ts = jsonObject.ts;
				if(ts=="" || ts == undefined)ts=new Date();
				var temp = jsonObject.T;
				var humidity = jsonObject.HR;

				document.getElementById("time").innerHTML = ts;
				var DS1 = document.gauges.get('dht-t');
				DS1.value = temp;

				var DS2 = document.gauges.get('dht-hr');
				DS2.value = humidity;	
				break;
			
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

