function startWebsocket() {
  var wsUri = "ws://" + location.host + "/";
  ws = new WebSocket(wsUri);
  ws.onopen = function(evt) { 
    console.log('websock open'); 
    ws.send("esp?\n");
  };
  ws.onclose = function(evt) {
    document.getElementById("output").innerHTML="Socket non connecté";	
    console.log('websock close'); 
  };
  ws.onerror = function(evt) { console.log(evt); };
  ws.onmessage = function(evt) {
    console.log("Message Rx: " + evt.data);
		
	if (typeof evt.data === "string") {
      try {
        // essai de créer un objet JSON 
        var jsonObject = JSON.parse(evt.data);
        switch(jsonObject.type) {
          case "info":    
          case "reseau":  
          case "ssid":                       
          case "chip": document.getElementById("output").innerHTML= evt.data;break;
          case "udp": document.getElementById("output").innerHTML= jsonObject.Port;break;  
          case "ADC":
          case "ADCHisto":document.getElementById("adc").innerHTML= evt.data;break;  
          case "BME280":
          case "BME280Histo": document.getElementById("bme").innerHTML= evt.data;break;    
          case "DHT22": 
          case "DHT22Histo": document.getElementById("dht22").innerHTML= evt.data;break; 
          case "DS1820": 
          case "DS1820Histo":document.getElementById("ds1820").innerHTML= evt.data;break;                               
        }   
      }
      catch( e ) {
        // pas du json  
        // affiche texte dans la zone output
        var ligne = evt.data;
        ligne = ligne.replace(/\n/g,"<br>")
        document.getElementById("output").innerHTML= ligne;
      }		
	  }    	
  };	

  commande.addEventListener('keypress', keyGotPressed, false);
  function keyGotPressed (e){
    var charCode = e.which;
    var cmd = document.getElementById('commande');
    if(charCode==13){
        console.log(cmd.value);
        ws.send(cmd.value);
    }
  };
};

// envoi de la commande de basculer l'état d'un GPIO
// format JSON
function flip(id) {
    ws.send("gpio flip "+id.substring(4) +"\n");
   // ws.send("{\"type\":\"gpio\",\"cmd\":\"flip\", \"id\":\"id.substring(4)\"}");
    console.log("{\"type\":\"gpio\",\"cmd\":\"flip\",\"id\":" +id.substring(4) +"\"}");
}
