#ifndef __HAIIRON__NETWORK__
#define __HAIIRON__NETWORK__

#include <avr/pgmspace.h>
#include "include.h"

// Setup the local wifi
const char* ssid     = "iptime";
const char* password = "appdevelopment";

/*-- basic 페이지 내용 --*/
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Clock Light</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Home Light Clock System</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element)
{
  var xhr = new XMLHttpRequest();
  if(element.checked)
  { xhr.open("GET", "/update?state=1", true); }
  else
  { xhr.open("GET", "/update?state=0", true); }
  xhr.send();
}

setInterval(function(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function()
  {
    if (this.readyState == 4 && this.status == 200)
    {
      var inputChecked;
      var outputStateM;
      if( this.responseText == 1)
      { 
        inputChecked = true;
        outputStateM = "On";
      }
      else
      { 
        inputChecked = false;
        outputStateM = "Off";
      }
      document.getElementById("output").checked = inputChecked;
      document.getElementById("outputState").innerHTML = outputStateM;
    }
  };
  xhttp.open("GET", "/state", true);
  xhttp.send();
}, 1000 ) ;
</script>
</body>
</html>
)rawliteral";
/*-- --------------------- --*/

String outputState()
{
  if (rvt)
  {
    return "checked";
  }
  else
    return "";
}

//Web
String Processor(const String& var)
{
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    String outputStateValue = outputState();
    String Clock = String(currentHour) + ":" + String(currentMinute);
    buttons += "<h4>Power - LED - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label><h3> Time " + Clock + "</h3>";
    return buttons;
  }
  return String();
}

#endif