#include <WiFi.h>

// Pin and Wi-Fi settings
#define ACS_PIN 34   // Pin where ACS712 is connected
const char* ssid = "IpTimeSSID";
const char* password = "Password";
const char* server_ip = "192.168.0.170";  // PC server IP address
const uint16_t server_port = 5000;  // Server port number

IPAddress local_ip(192, 168, 0, 100);  // Static IP for ESP32
IPAddress gateway(192, 168, 0, 1);  // Default gateway
IPAddress subnet(255, 255, 255, 0);  // Subnet mask

WiFiClient client;

// Calibration variables
float zeroCurrentVoltage = 0;  // Voltage at zero current (calibration result)
float calibration_factor = 0.066;  // Calibration factor for ACS712 30A module

// Connect to the TCP server
bool connectToServer() {
  Serial.print("Attempting to connect to the server at ");
  Serial.print(server_ip);
  Serial.print(":");
  Serial.println(server_port);

  if (client.connect(server_ip, server_port)) {
    Serial.println("Connected to the server");
    return true;
  } else {
    Serial.println("Failed to connect to the server. Check IP and port.");
    return false;
  }
}

// Check Wi-Fi connection and reconnect if necessary
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
    WiFi.disconnect();
    WiFi.config(local_ip, gateway, subnet);  // Static IP configuration
    WiFi.begin(ssid, password);  // Reconnect
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Reconnected to Wi-Fi");
  }
}

// Function to perform calibration by taking the average of 10 readings
void performCalibration() {
  float totalVoltage = 0;
  for (int i = 0; i < 10; i++) {
    int sensorValue = analogRead(ACS_PIN);
    float voltage = sensorValue * (3.3 / 4095.0);  // Convert ADC value to voltage
    totalVoltage += voltage;
    delay(100);  // Short delay between readings
  }
  zeroCurrentVoltage = totalVoltage / 10.0;  // Average value to find zero-current voltage
  Serial.print("Calibration complete. Zero current voltage: ");
  Serial.println(zeroCurrentVoltage);
}

// Function to read current and send it to the server
void readAndSendCurrent() {
  // Check Wi-Fi connection
  checkWiFiConnection();

  // Ensure server connection
  if (!client.connected()) {
    Serial.println("Disconnected from server. Reconnecting...");
    while (!connectToServer()) {
      delay(5000);  // Retry every 5 seconds
    }
  }

  // Read current from ACS712
  int sensorValue = analogRead(ACS_PIN);  // Read from pin
  float voltage = sensorValue * (3.3 / 4095.0);  // Convert ADC value to voltage
  float current = (voltage - zeroCurrentVoltage) / calibration_factor;  // Apply calibration

  // Print the current value
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" A");

  // Send the current value to the server
  client.print(String(current) + "\n");
}

void setup() {
  Serial.begin(115200);

  // Disable Wi-Fi power saving mode
  WiFi.setSleep(false);

  // Connect to Wi-Fi with static IP
  WiFi.config(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Still connecting...");
  }

  Serial.println("Connected to Wi-Fi");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  // Attempt to connect to the TCP server
  while (!connectToServer()) {
    delay(5000);  // Retry every 5 seconds
  }

  // Perform calibration 10 times
  performCalibration();

  Serial.println("\n End of Calibration and Sleep for 10 seconds");

  delay(10000);
}

void loop() {
  // Read and send current every 0.002 seconds (approximately 500 times per second)
  readAndSendCurrent();
  delay(2);  // 0.002-second delay
}


