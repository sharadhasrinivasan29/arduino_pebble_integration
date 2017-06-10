var url = " ";
var ipAddress = "158.130.221.99"; // Hard coded IP address
var port = "3002"; // Same port specified as argument to server
var key = " ";


Pebble.addEventListener("appmessage", function(e) {
     if (e.payload) {
        if (e.payload.recent_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "0";
          key = "0";
          sendToServer();
       }
        else if (e.payload.fahrenheit) {
          url = "http://" + ipAddress + ":" + port + "/"+ "1";
          key = "1";
          sendToServer();
       }
        else if (e.payload.celsius) {
          url = "http://" + ipAddress + ":" + port + "/"+ "2";
          key = "2";
          sendToServer();
       }
        else if (e.payload.average_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "3";
          key = "3";
          sendToServer();
       }
        else if (e.payload.high_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "4";
          key = "4";
          sendToServer();
       }
        else if (e.payload.low_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "5";
          key = "5";
          sendToServer();
       }
        else if (e.payload.enter_standby) {
          url = "http://" + ipAddress + ":" + port + "/"+ "6";
          key = "6";
          sendToServer();
       }
        else if (e.payload.rising_falling) {
          url = "http://" + ipAddress + ":" + port + "/"+ "7";
          key = "7";
          sendToServer();
       }
        else if (e.payload.print_word) {
          url = "http://" + ipAddress + ":" + port + "/"+ "8";
          key = "8";
          sendToServer();
       }
        else if (e.payload.comfy_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "9";
          key = "9";
          sendToServer();
       }
        else if (e.payload.cold_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "a";
          key = "10";
          sendToServer();
       }
        else if (e.payload.hot_temp) {
          url = "http://" + ipAddress + ":" + port + "/"+ "b";
          key = "11";
          sendToServer();
       }
        else if (e.payload.drink) {
          url = "http://" + ipAddress + ":" + port + "/"+ "c";
          key = "12";
          sendToServer();
       }
        else if (e.payload.exiting_standby) {
          url = "http://" + ipAddress + ":" + port + "/" + "d";
          key = "13";
          sendToServer();
       }
        else if (e.payload.word2) {
          url = "http://" + ipAddress + ":" + port + "/"+ "e";
          key = "14";
          sendToServer();
       }       
        else if (e.payload.word3) {
          url = "http://" + ipAddress + ":" + port + "/"+ "f";
          key = "15";
          sendToServer();
       }      
        else if (e.payload.select_hot) {
          url = "http://" + ipAddress + ":" + port + "/"+ "g";
          key = "16";
          sendToServer();
       }
        else if (e.payload.c_35) {
          url = "http://" + ipAddress + ":" + port + "/"+ "h";
          key = "17";
          sendToServer();
       }
        else if (e.payload.c_30) {
          url = "http://" + ipAddress + ":" + port + "/"+ "i";
          key = "18";
          sendToServer();
       }
        else if (e.payload.c_25) {
          url = "http://" + ipAddress + ":" + port + "/"+ "j";
          key = "19";
          sendToServer();
       }
        else if (e.payload.select_cold) {
          url = "http://" + ipAddress + ":" + port + "/"+ "k";
          key = "20";
          sendToServer();
       }
        else if (e.payload.c_0) {
          url = "http://" + ipAddress + ":" + port + "/"+ "l";
          key = "21";
          sendToServer();
       }
        else if (e.payload.c_5) {
          url = "http://" + ipAddress + ":" + port + "/"+ "m";
          key = "22";
          sendToServer();
        }
        else {
          url = "http://" + ipAddress + ":" + port + "/" + "n";
          key = "13";
          sendToServer();
       }
     }
    else Pebble.sendAppMessage({ "0": "nopayload" });    
  }
);



function sendToServer() {
  var req = new XMLHttpRequest();
  var method = "GET";
  var async = true;
  req.onload = function(e) {
  // see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText);
  if (response) {
    if (response.name) {
      msg = response.name;
      if (msg.length === 0) {
        msg = "Disconnected from Arduino.";
      }
    }
    else msg = "noname";
  }
// sends message back to pebble
    if(key == "0") {
      Pebble.sendAppMessage({ "0": msg });
    }
    else if(key == "1") {
      Pebble.sendAppMessage({ "1": msg });
    }
    else if(key == "2") {
      Pebble.sendAppMessage({ "2": msg });
    }
    else if(key == "3") {
      Pebble.sendAppMessage({ "3": msg });
    }
    else if(key == "4") {
      Pebble.sendAppMessage({ "4": msg });
    }
    else if(key == "5") {
      Pebble.sendAppMessage({ "5": msg });
    }
    else if(key == "6") {
      Pebble.sendAppMessage({ "6": msg });
    }
    else if(key == "7") {
      Pebble.sendAppMessage({ "7": msg });
    }
    else if(key == "8") {
      Pebble.sendAppMessage({ "8": msg });
    }
    else if(key == "9") {
      Pebble.sendAppMessage({ "9": msg });
    }
    else if(key == "10") {
      Pebble.sendAppMessage({ "10": msg });
    }
    else if(key == "11") {
      Pebble.sendAppMessage({ "11": msg });
    }
    else if(key == "12") {
      Pebble.sendAppMessage({ "12": msg });
    }
    else if(key == "13") {
      Pebble.sendAppMessage({ "13": msg });
    }
    else if(key == "14") {
      Pebble.sendAppMessage({ "14": msg });
    }
    else if(key == "15") {
      Pebble.sendAppMessage({ "15": msg });
    }
    else if(key == "16") {
      Pebble.sendAppMessage({ "16": msg });
    }
    else if(key == "17") {
      Pebble.sendAppMessage({ "17": msg });
    }
    else if(key == "18") {
      Pebble.sendAppMessage({ "18": msg });
    }
    else if(key == "19") {
      Pebble.sendAppMessage({ "19": msg });
    }
    else if(key == "20") {
      Pebble.sendAppMessage({ "20": msg });
    }
    else if(key == "21") {
      Pebble.sendAppMessage({ "21": msg });
    }
    else if(key == "22") {
      Pebble.sendAppMessage({ "22": msg });
    }
    else if(key == "23") {
      Pebble.sendAppMessage({ "23": msg });
    }
};
  
  // Handle error in http request (e.g. Unable to connect to server)
  // Just pass on error key to the watch app
  req.onerror = function(e) {
       Pebble.sendAppMessage({"0" : "Cannot connect to server."});
       Pebble.sendAppMessage({"3" : "Cannot connect to server."});
       Pebble.sendAppMessage({"4" : "Cannot connect to server."});
       Pebble.sendAppMessage({"5" : "Cannot connect to server."});
       Pebble.sendAppMessage({"7" : "Cannot connect to server."});
       Pebble.sendAppMessage({"12" : "Cannot connect to server."});
  };
  
  
 req.open(method, url, async);
 req.send(null);
}