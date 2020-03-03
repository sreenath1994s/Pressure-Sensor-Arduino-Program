const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
 
<head>
  <title>Pressure Sensor Graph</title>
  <script src = "http://192.168.4.1:81/Chart.min.js"></script> 
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }
 
  /* Data Table Styling */
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
  }
 
  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }
 
  #dataTable tr:nth-child(even){background-color: #f2f2f2;}
 
  #dataTable tr:hover {background-color: #ddd;}
 
  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: left;
    background-color: rgba( 153, 103, 254, 1);
    color: white;
  }
  </style>
</head>
 
<body>
    <div style="text-align:center; color: #444444;margin: 10px auto 10px; font-family: Helvetica}"><b>Pressure Sensor Data Interface</b></div>
    <div class="chart-container" position: relative; height:350px; width:100%">
        <canvas id="Chart" width="400" height="400"></canvas>
    </div>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>Pressure1</th><th>Pressure2</th><th>Pressure3</th><th>Pressure4</th></tr>
  </table>
</div>
<br>
<br>  
 
<script>
//Graphs primary reference is chart.js
var pressureval1 = [];
var pressureval2 = [];
var pressureval3 = [];
var pressureval4 = [];
var timeStamp = [];
var ctx = document.getElementById("Chart").getContext('2d');
    var Chart2 = new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeStamp,  //Bottom Labeling
            datasets: [{
                label: "Pressure 1", //Pressure 1 Red
                fill: false,  //Try with true
                backgroundColor: 'rgba( 254, 99, 131, 1)', //Dot marker color
                borderColor: 'rgba( 254, 99, 131, 1)', //Graph Line Color
                data: pressureval1,
            },{
                label: "Pressure 2", //Pressure 2 Green
                fill: false,  //Try with true
                backgroundColor: 'rgba(75, 192, 187, 1)', //Dot marker color
                borderColor: 'rgba(75, 192, 187, 1)', //Graph Line Color
                data: pressureval2,
            },{
                label: "Pressure 3", //Pressure 2 Yellow
                fill: false,  //Try with true
                backgroundColor: 'rgba(255, 220, 46, 1)', //Dot marker color
                borderColor: 'rgba(255, 220, 46, 1)', //Graph Line Color
                data: pressureval3,
            },
            {
                label: "Pressure 4", //Pressure 2 Blue
                fill: false,  //Try with true
                backgroundColor: 'rgba( 52, 162, 241, 1)', //Dot marker color
                borderColor: 'rgba( 52, 162, 241, 1)', //Graph Line Color
                data: pressureval4,
            }],
        },
        options: {
            title: {
                    display: false,
                    text: "Readings of Pressure"
                },
            maintainAspectRatio: false,
            elements: {
            line: {
                    tension: 0.5 //Smoothening (Curved) of data lines
                }
            },
            scales: {
                    yAxes: [{
                        ticks: {
                            beginAtZero:true
                        }
                    }]
            }
        }
    });
 
//On Page load show graphs
window.onload = function() {
  console.log(new Date().toLocaleTimeString());
};
 
//Ajax script to get sensor data at every 0.1 Seconds 
 
setInterval(function() {
  // Call a function repetatively with 5 Second interval
  getData();
}, 500); //500mSeconds update rate
 
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString();
  var txt = this.responseText;
  var obj = JSON.parse(txt); //Ref: https://www.w3schools.com/js/js_json_parse.asp
  if(timeStamp.length != 40)
  {
      pressureval1.push(obj.Pressure1);
      pressureval2.push(obj.Pressure2);
      pressureval3.push(obj.Pressure3);
      pressureval4.push(obj.Pressure4);
      timeStamp.push(time);
      Chart2.update();  //Update Graphs
  }
  else
  {
     pressureval1.shift();
     pressureval2.shift();
     pressureval3.shift();
     pressureval4.shift();
     timeStamp.shift();
     pressureval1.push(obj.Pressure1);
     pressureval2.push(obj.Pressure2);
     pressureval3.push(obj.Pressure3);
     pressureval4.push(obj.Pressure4);
     timeStamp.push(time);
     Chart2.update();  //Update Graphs
  }
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1); //Add after headings
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);
    var cell5 = row.insertCell(4);
    cell1.innerHTML = time;
    cell2.innerHTML = obj.Pressure1;
    cell3.innerHTML = obj.Pressure2;
    cell4.innerHTML = obj.Pressure3;
    cell5.innerHTML = obj.Pressure4;
    }
  };
  xhttp.open("GET", "data", true); //Handle data from server on ESP32
  xhttp.send();
}
    
</script>
</body>
 
</html>
 
)=====";
