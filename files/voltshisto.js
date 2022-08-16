// Initialize WebSocket connection and event handlers
function setupWebsocket() {
	var ctx = document.getElementById('canvas').getContext('2d');
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
	ws = new WebSocket(wsUri);

	var qtePoints =24; // qte de points du graphe à l"écran
		
	ws.onopen = function(e) {
		console.log("Client en ligne,requête adc histo");
		// requête pour dernières valeurs
		ws.send("adc histo\n");
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
					var temps = jsonObject.ts; 
					if(temps == "")temps = new Date();
							
					for(i=0;i<lineChartData.datasets.length;i++){
						if(lineChartData.datasets[i].data.length >=qtePoints){
							lineChartData.datasets[i].data.shift();
						}
					}
					lineChartData.datasets[0].data.push({x:temps,y:volts});	
					window.myLine.update();
					
				}

				if(jsonObject.type=="ADCHisto"){
					//log("Message received: " + e.data);
					// Extraire les valeurs des clés
					var temps = jsonObject.ts;
					var volts = jsonObject.volts;
					
					// copie les valeurs dans les datasets
					for(i=0;i<temps.length;i++){
							lineChartData.datasets[0].data[i]={"x":temps[i],"y":volts[i]};
					}						
					window.myLine.update();			
				}

				if(jsonObject.type=="noRTC"){
					var t = new Date() -24000
					for(i=0;i<qtePoints;i++){
						temps[i]=t+i*1000;
						lineChartData.datasets[0].data[i]={"x":temps[i],"y":volts[i]};
					}	
					
					window.myLine.update();		
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



