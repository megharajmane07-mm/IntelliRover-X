#include <WiFi.h> 
#include <WebServer.h> 
// Pin assignments 
#define TRIG_PIN 5   // Ultrasonic trigger (D5) 
#define ECHO_PIN 2   // Ultrasonic echo (D2) 
#define ENA_PIN 13   // Motor A (left) enable (D13) 
#define IN1_PIN 14   // Motor A input 1 (D14) 
#define IN2_PIN 12   // Motor A input 2 (D12) 
#define IN3_PIN 27   // Motor B input 1 (D27) 
#define IN4_PIN 26   // Motor B input 2 (D26) 
#define ENB_PIN 25   // Motor B (right) enable (D25) 
// WiFi credentials for Access Point mode 
const char* ssid = "ESP32_RC"; 
const char* password = "esp32pass"; 
// Create a web server on port 80 
WebServer server(80); 
// Global flag for mode selection: if true then ultrasonic sensor is 
active 
bool useUltrasonic = false; 
// Set distance threshold to 50 cm – if an object is closer than this, the 
car will reverse until it reaches at least 51 cm 
const int thresholdDistance = 50; 
unsigned long lastUltrasonicCheck = 0; 
const unsigned long ultrasonicInterval = 100; // check sensor every 100ms 
// Forward declarations 
void handleRoot(); 
void handleCommand(); 
void handleToggle(); 
void moveForward(); 
void moveBackward(); 
void turnLeft(); 
void turnRight(); 
void stopMotors(); 
long readUltrasonicDistance(); 
void setup() { 
Serial.begin(115200); 
// Initialize pins 
pinMode(TRIG_PIN, OUTPUT); 
pinMode(ECHO_PIN, INPUT); 
pinMode(ENA_PIN, OUTPUT); 
pinMode(ENB_PIN, OUTPUT); 
pinMode(IN1_PIN, OUTPUT); 
pinMode(IN2_PIN, OUTPUT); 
pinMode(IN3_PIN, OUTPUT); 
pinMode(IN4_PIN, OUTPUT); 
// Ensure motors are stopped at startup 
stopMotors(); 
// Start WiFi in Access Point mode 
WiFi.softAP(ssid, password); 
Serial.println("Access Point Started"); 
Serial.print("AP IP address: "); 
Serial.println(WiFi.softAPIP()); 
// Setup web server endpoints 
server.on("/", handleRoot);           
custom joystick UI 
// Serves the control page with 
server.on("/command", handleCommand);   // Receives joystick commands 
server.on("/toggle", handleToggle);     
// Receives toggle mode command 
server.begin(); 
Serial.println("Web server started"); 
} 
void loop() { 
server.handleClient(); 
// If the ultrasonic mode is enabled, check the sensor periodically 
if (useUltrasonic && (millis() - lastUltrasonicCheck > 
ultrasonicInterval)) { 
lastUltrasonicCheck = millis(); 
long distance = readUltrasonicDistance(); 
Serial.print("Distance: "); 
Serial.print(distance); 
Serial.println(" cm"); 
// If an object is detected within the threshold, reverse until the 
object is at least 51 cm away 
if (distance > 0 && distance < thresholdDistance) { 
Serial.println("Object detected! Reversing until distance >= 51 
cm..."); 
while (readUltrasonicDistance() < 51) { 
moveBackward(); 
delay(50);  // short delay to allow sensor readings to update 
} 
stopMotors(); 
} 
} 
} 
// Serve the main HTML page with a custom joystick UI 
void handleRoot() { 
String html = "<!DOCTYPE html>" 
"<html>" 
"<head>" 
"<meta name='viewport' content='width=device-width, 
initial-scale=1'>" 
"<title>Multifunction Car By Robotskull</title>" 
"<style>" 
"body { font-family: Arial, sans-serif; margin:0; padding:0; " 
"background: linear-gradient(135deg, #72EDF2 10%, #5151E5 
100%); color: #fff; }" 
".container { text-align: center; padding: 20px; }" 
"h1 { font-size: 2.5em; margin-bottom: 5px; }" 
"h2 { font-size: 1.5em; margin-bottom: 20px; }" 
".toggle { margin-bottom: 20px; font-size: 1.2em; }" 
"#joystick-container { position: relative; width: 300px; height: 
300px; margin: 20px auto; touch-action: none; }" 
"#joystick-bg { position: absolute; width: 100%; height: 100%; 
border: 2px solid rgba(255,255,255,0.7); border-radius: 50%; }" 
"#joystick-stick { position: absolute; width: 80px; height: 80px; 
background: rgba(255,255,255,0.9); " 
"border-radius: 50%; top: 50%; left: 50%; 
transform: translate(-50%, -50%); " 
"box-shadow: 0 4px 6px rgba(0,0,0,0.3); 
transition: transform 0.1s ease-out; }" 
".footer { margin-top: 40px; font-size: 0.9em; }" 
"</style>" 
"</head>" 
"<body>" 
"<div class='container'>" 
"<h1>Multifunction Car</h1>" 
"<h2>By Robotskull</h2>" 
"<div class='toggle'>" 
"<label for='toggleMode'>Ultrasonic + Remote Mode:</label>" 
"<input type='checkbox' id='toggleMode' 
onchange='toggleMode()'>" 
"</div>" 
"<div id='joystick-container'>" 
"<div id='joystick-bg'></div>" 
"<div id='joystick-stick'></div>" 
"</div>" 
"<div class='footer'>&copy; 2025 Robotskull</div>" 
"</div>" 
"<script>" 
// Toggle mode function 
"function toggleMode() {" 
"  var mode = document.getElementById('toggleMode').checked ? 
'ultrasonic' : 'remote';" 
"  fetch('/toggle?state=' + mode)" 
"    
.then(response => response.text())" 
"    
"}" 
.then(data => console.log(data));" 
"var container = document.getElementById('joystick-container');" 
"var stick = document.getElementById('joystick-stick');" 
"var lastCommand = 'stop';" 
"function sendCommand(cmd) {" 
"  if (cmd !== lastCommand) {" 
"    
lastCommand = cmd;" 
"    
fetch('/command?cmd=' + cmd)" 
        "      .then(response => response.text())" 
        "      .then(data => console.log('Command:', cmd));" 
        "  }" 
        "}" 
        "function handleMove(evt) {" 
        "  evt.preventDefault();" 
        "  var containerRect = container.getBoundingClientRect();" 
        "  var centerX = containerRect.width / 2;" 
        "  var centerY = containerRect.height / 2;" 
        "  var posX = 0, posY = 0;" 
        "  if(evt.touches && evt.touches.length > 0) {" 
        "    posX = evt.touches[0].clientX - containerRect.left;" 
        "    posY = evt.touches[0].clientY - containerRect.top;" 
        "  } else {" 
        "    posX = evt.clientX - containerRect.left;" 
        "    posY = evt.clientY - containerRect.top;" 
        "  }" 
        "  var deltaX = posX - centerX;" 
        "  var deltaY = posY - centerY;" 
        "  var distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);" 
        "  var maxDistance = centerX;"  // Assuming square container 
        "  if (distance > maxDistance) {" 
        "    var ratio = maxDistance / distance;" 
        "    deltaX *= ratio;" 
        "    deltaY *= ratio;" 
        "    distance = maxDistance;" 
        "  }" 
        // Update stick position while keeping it centered (using calc) 
        "  stick.style.transform = 'translate(calc(-50% + ' + deltaX + 
'px), calc(-50% + ' + deltaY + 'px))';" 
        "  if(distance < 10) { sendCommand('stop'); return; }" 
        "  var angleRad = Math.atan2(deltaY, deltaX);" 
        "  var angleDeg = angleRad * (180 / Math.PI);" 
        "  var command = 'stop';" 
        "  if(angleDeg >= -135 && angleDeg <= -45) {" 
        "    command = 'forward';" 
        "  } else if(angleDeg > -45 && angleDeg < 45) {" 
        "    command = 'left';"       // Swapped: Right becomes Left 
        "  } else if(angleDeg >= 45 && angleDeg <= 135) {" 
        "    command = 'backward';" 
"  } else {" 
"    
command = 'right';"      
"  }" 
"  sendCommand(command);" 
"}" 
"function resetJoystick() {" 
// Swapped: Left becomes Right 
"  stick.style.transform = 'translate(-50%, -50%)';" 
"  sendCommand('stop');" 
"}" 
"container.addEventListener('touchstart', handleMove, false);" 
"container.addEventListener('touchmove', handleMove, false);" 
"container.addEventListener('touchend', resetJoystick, false);" 
"container.addEventListener('mousedown', function(evt) {" 
"  container.addEventListener('mousemove', handleMove, false);" 
"});" 
"container.addEventListener('mouseup', function(evt) {" 
"  container.removeEventListener('mousemove', handleMove, false);" 
"  resetJoystick();" 
"});" 
"</script>" 
"</body>" 
"</html>"; 
server.send(200, "text/html", html); 
} 
// Handle remote control commands from the webpage 
void handleCommand() { 
if (server.hasArg("cmd")) { 
String cmd = server.arg("cmd"); 
Serial.print("Command received: "); 
Serial.println(cmd); 
if (cmd == "forward") { 
moveForward(); 
} else if (cmd == "backward") { 
moveBackward(); 
} else if (cmd == "left") { 
turnLeft(); 
} else if (cmd == "right") { 
turnRight(); 
} else if (cmd == "stop") { 
stopMotors(); 
} 
server.send(200, "text/plain", "OK"); 
} else { 
server.send(400, "text/plain", "Bad Request"); 
} 
} 
// Handle toggle mode command to switch between ultrasonic+remote and 
remote-only 
void handleToggle() { 
if (server.hasArg("state")) { 
String state = server.arg("state"); 
Serial.print("Toggle mode state: "); 
Serial.println(state); 
useUltrasonic = (state == "ultrasonic"); 
server.send(200, "text/plain", "Mode updated"); 
} else { 
server.send(400, "text/plain", "Bad Request"); 
} 
} 
// Motor control functions 
void moveForward() { 
// Set both motors to move forward 
digitalWrite(IN1_PIN, HIGH); 
digitalWrite(IN2_PIN, LOW); 
digitalWrite(IN3_PIN, HIGH); 
digitalWrite(IN4_PIN, LOW); 
digitalWrite(ENA_PIN, HIGH); 
digitalWrite(ENB_PIN, HIGH); 
} 
void moveBackward() { 
// Set both motors to move backward 
digitalWrite(IN1_PIN, LOW); 
digitalWrite(IN2_PIN, HIGH); 
digitalWrite(IN3_PIN, LOW); 
digitalWrite(IN4_PIN, HIGH); 
digitalWrite(ENA_PIN, HIGH); 
digitalWrite(ENB_PIN, HIGH); 
} 
void turnLeft() { 
// For a left turn, disable (or slow) the left motor and run the right 
motor forward 
digitalWrite(IN1_PIN, LOW); 
digitalWrite(IN2_PIN, LOW); 
digitalWrite(IN3_PIN, HIGH); 
digitalWrite(IN4_PIN, LOW); 
digitalWrite(ENA_PIN, LOW); 
digitalWrite(ENB_PIN, HIGH); 
} 
void turnRight() { 
// For a right turn, disable (or slow) the right motor and run the left 
motor forward 
digitalWrite(IN1_PIN, HIGH); 
digitalWrite(IN2_PIN, LOW); 
digitalWrite(IN3_PIN, LOW); 
digitalWrite(IN4_PIN, LOW); 
digitalWrite(ENA_PIN, HIGH); 
digitalWrite(ENB_PIN, LOW); 
} 
void stopMotors() { 
// Stop all motor activity 
digitalWrite(IN1_PIN, LOW); 
digitalWrite(IN2_PIN, LOW); 
digitalWrite(IN3_PIN, LOW); 
digitalWrite(IN4_PIN, LOW); 
digitalWrite(ENA_PIN, LOW); 
digitalWrite(ENB_PIN, LOW); 
} 
// Function to read distance from the ultrasonic sensor (in centimeters) 
long readUltrasonicDistance() { 
digitalWrite(TRIG_PIN, LOW); 
delayMicroseconds(2); 
digitalWrite(TRIG_PIN, HIGH); 
delayMicroseconds(10); 
digitalWrite(TRIG_PIN, LOW); 
long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout after 30ms 
long distance = duration / 58.2; // Convert time (µs) to distance in cm 
return distance; 
}