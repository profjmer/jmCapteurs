// Initialize WebSocket connection and event handlers
function setupWebsocket() {
	var ctx = document.getElementById('canvas').getContext('2d');
	var wsUri = "ws://" + location.host + "/"; // pour ESP8266
	ws = new WebSocket(wsUri);

	var qtePoints =24; // qte de points du graphe à l"écran
		
	ws.onopen = function(e) {
		console.log("Client en ligne,requête ds1820 histo");
		// requête pour dernières valeurs
		ws.send("ds histo\n");
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
			
				if(jsonObject.type=="DS1820"){
					
					// Extraire les valeurs des clés
					var T = jsonObject.T;			
					var temps = jsonObject.ts; 
					var dsCount = T.length;
					if(temps=="")temps = new Date(); 
				
					// shift points
					for(i=0;i<lineChartData.datasets.length;i++){
						if(lineChartData.datasets[i].data.length >=qtePoints){
							lineChartData.datasets[i].data.shift();
						}
					}
					// insert point
					switch(dsCount){
						case 4:
								lineChartData.datasets[0].data.push({x:temps,y:T[0]});
								lineChartData.datasets[1].data.push({x:temps,y:T[1]});
								lineChartData.datasets[2].data.push({x:temps,y:T[2]});			
								lineChartData.datasets[3].data.push({x:temps,y:T[3]});		
								
								myLine.scales["y-axis-4"].options.display=true;			// y-axis-1 toujours visible
								myLine.scales["y-axis-3"].options.display=true;				
								myLine.scales["y-axis-2"].options.display=true;	
								/*
								myLine.legend.legendItems[3].hidden=false;								
								myLine.legend.legendItems[2].hidden=false;
								myLine.legend.legendItems[1].hidden=false;	*/
								break;
						case 3:
								lineChartData.datasets[0].data.push({x:temps,y:T[0]});
								lineChartData.datasets[1].data.push({x:temps,y:T[1]});
								lineChartData.datasets[2].data.push({x:temps,y:T[2]});			

								myLine.scales["y-axis-4"].options.display=false;						
								myLine.scales["y-axis-3"].options.display=true;				
								myLine.scales["y-axis-2"].options.display=true;	
								/*
								myLine.legend.legendItems[3].hidden=true;								
								myLine.legend.legendItems[2].hidden=false;
								myLine.legend.legendItems[1].hidden=false;	*/
								
								break;
						case 2:
								lineChartData.datasets[0].data.push({x:temps,y:T[0]});
								lineChartData.datasets[1].data.push({x:temps,y:T[1]});	
								
								myLine.scales["y-axis-4"].options.display=false;	
								myLine.scales["y-axis-3"].options.display=false;		
								myLine.scales["y-axis-2"].options.display=true;										
								/*
								myLine.legend.legendItems[3].hidden=true;								
								myLine.legend.legendItems[2].hidden=true;
								myLine.legend.legendItems[1].hidden=false;	*/
	

								
								break;
						case 1:
								lineChartData.datasets[0].data.push({x:temps,y:T[0]});	
								myLine.scales["y-axis-4"].options.display=false;	
								myLine.scales["y-axis-3"].options.display=false;				
								myLine.scales["y-axis-2"].options.display=false;	
								/*
								myLine.legend.legendItems[3].hidden=true;								
								myLine.legend.legendItems[2].hidden=true;
								myLine.legend.legendItems[1].hidden=true;						*/		
								break;
					}
						
					window.myLine.update();			
				}

				if(jsonObject.type=="noRTC"){
					var t = new Date() -24000
					for(i=0;i<qtePoints;i++){
						temps[i]=t+i*1000;
						lineChartData.datasets[0].data.push({x:temps,y:T[0]});
						lineChartData.datasets[1].data.push({x:temps,y:T[1]});
						lineChartData.datasets[2].data.push({x:temps,y:T[2]});			
						lineChartData.datasets[3].data.push({x:temps,y:T[3]});	
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



