//Custom function for serving sensor control webpage
String SendHTML(uint8_t senvar){

  String ptr;

  if(senvar==2){
  ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head> <meta http-equiv=\"refresh\" content=\"15; url='http://192.168.4.1/'\"/>\n";
  ptr +="<title>Pressure Sensor Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Resetting sensor, page will refresh in 15 seconds.</h1>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }
  else{
  ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Pressure Sensor Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Pressure Sensor Control Interface</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
   if(senvar==0)
  {ptr +="<p>Sensor Status: OFF</p><a class=\"button button-on\" href=\"/sensoron\">ON</a>\n";}
  else
  {ptr +="<p>Sensor Status: ON</p><a class=\"button button-off\" href=\"/sensoroff\">OFF</a>\n";}
  
  ptr +="<p>Download Data</p><a class=\"button button-on\" href=\"/download\">Data</a>\n";
  
  ptr +="<p>Restart System. <span style=\"color:red\">Caution download data first !!!</span></p><a class=\"button button-on\" href=\"/reset\">Restart</a>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }
}
