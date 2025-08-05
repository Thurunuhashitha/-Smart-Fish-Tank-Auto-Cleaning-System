// These are defined by your Blynk project
#define BLYNK_TEMPLATE_ID "TMPL6eIWVRruS"
#define BLYNK_TEMPLATE_NAME "smart fish tank"
#define BLYNK_AUTH_TOKEN "mwbYHTdKp1NixDfL18yYIF7QX2VDaAIc"

// Replace these with your Wi-Fi credentials
char ssid[] = "Nevil’s iPhone";
char pass[] = "Hemantha@99";

void setup()
{
  Serial.begin(115200);                   // Start serial monitor
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Connect ESP32 to Blynk via WiFi
}

void loop()
{
  Blynk.run(); // Keeps the connection alive
}

// Water Tank Monitoring and Cleaning System
// Sensors: Ultrasonic (D13 trigger, D12 echo), pH sensor (D32), Temperature sensor (D18)
// Actuators: Water pump (D14), Solenoid valve (D33)

#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
#define TRIG_PIN 13
#define ECHO_PIN 12
#define PH_SENSOR_PIN 32
#define TEMP_SENSOR_PIN 18
#define WATER_PUMP_PIN 14
#define SOLENOID_VALVE_PIN 33

// Temperature sensor setup
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temperatureSensor(&oneWire);

// Comfortable zone parameters
#define PH_MIN 9.0
#define PH_MAX 11.0
#define TEMP_MIN 25.0
#define TEMP_MAX 30.0

// Water level parameters (in cm)
#define FULL_WATER_LEVEL 5    // 100% water level (5cm from sensor)
#define HALF_WATER_LEVEL 10   // 50% water level (10cm from sensor)

// System variables
float currentPH = 0;
float currentTemp = 0;
float waterLevel = 0;
bool systemInDangerZone = false;
bool cleaningInProgress = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  
  // Initialize temperature sensor
  temperatureSensor.begin();
  
  // Ensure all actuators are OFF initially
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(SOLENOID_VALVE_PIN, LOW);
  
  Serial.println("Water Tank Monitoring System Initialized");
  Serial.println("Comfortable Zone: pH 9-11, Temperature 25-30°C");
  Serial.println("==========================================");
  
  delay(500);
}

void loop() {
  // Read all sensors
  readSensors();
  
  // Display current readings
  displayReadings();
  
  // Check if system is in danger zone
  checkDangerZone();
  
  // Start cleaning if needed
  if (systemInDangerZone && !cleaningInProgress) {
    startCleaningProcess();
  }
  
  delay(2000); // Check every 2 seconds
}

void readSensors() {
  // Read ultrasonic sensor for water level
  waterLevel = readUltrasonicDistance();
  
  // Read pH sensor
  currentPH = readPHSensor();
  
  // Read temperature (assuming it's from pH sensor or separate temp sensor)
  currentTemp = readTemperature();
}

float readUltrasonicDistance() {
  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read the echo
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate distance in cm
  float distance = (duration * 0.034) / 2;
  
  return distance;
}

float readPHSensor() {
  // Read analog value from pH sensor
  int analogValue = analogRead(PH_SENSOR_PIN);
  
  // Convert to pH value (calibration needed based on your specific sensor)
  // This is a basic conversion - you may need to calibrate
  float voltage = analogValue * (3.3 / 4095.0); // For ESP32 12-bit ADC
  float ph = 3.5 * voltage + 0.0; // Basic linear conversion - adjust based on calibration
  
  return ph;
}

float readTemperature() {
  // Request temperature from DS18B20 sensor on pin D18
  temperatureSensor.requestTemperatures();
  
  // Read temperature in Celsius
  float temperature = temperatureSensor.getTempCByIndex(0);
  
  // Check if reading is valid
  if (temperature == DEVICE_DISCONNECTED_C) {
    Serial.println("⚠️ Temperature sensor disconnected!");
    return 25.0; // Return safe default value
  }
  
  return temperature;
}

void displayReadings() {
  Serial.println("Current Readings:");
  Serial.print("Water Level: ");
  Serial.print(waterLevel);
  Serial.println(" cm (distance to water)");
  
  Serial.print("pH Level: ");
  Serial.println(currentPH, 2);
  
  Serial.print("Temperature: ");
  Serial.print(currentTemp, 1);
  Serial.println("°C");
  
  Serial.print("Status: ");
  if (systemInDangerZone) {
    Serial.println("🚨 DANGER ZONE DETECTED!");
  } else {
    Serial.println("✅ Normal Operation");
  }
  Serial.println("------------------");
}

void checkDangerZone() {
  bool phInDanger = (currentPH < PH_MIN || currentPH > PH_MAX);
  bool tempInDanger = (currentTemp < TEMP_MIN || currentTemp > TEMP_MAX);
  
  systemInDangerZone = phInDanger || tempInDanger;
  
  if (systemInDangerZone) {
    Serial.println("⚠️ DANGER ZONE DETECTED:");
    if (phInDanger) {
      Serial.print("  pH out of range: ");
      Serial.print(currentPH);
      Serial.println(" (Safe: 9.0-11.0)");
    }
    if (tempInDanger) {
      Serial.print("  Temperature out of range: ");
      Serial.print(currentTemp);
      Serial.println("°C (Safe: 25.0-30.0°C)");
    }
  }
}

void startCleaningProcess() {
  cleaningInProgress = true;
  Serial.println("\n🔧 STARTING CLEANING PROCESS...");
  
  // Step 1: Remove 50% water (drain to 10cm level)
  Serial.println("Step 1: Removing 50% water...");
  drainWaterTo50Percent();
  
  // Step 2: Fill water to 100% (5cm level)
  Serial.println("Step 2: Filling fresh water to 100%...");
  fillWaterTo100Percent();
  
  Serial.println("✅ CLEANING PROCESS COMPLETED");
  Serial.println("System returning to normal monitoring...\n");
  
  cleaningInProgress = false;
  delay(500);
}

void drainWaterTo50Percent() {
  Serial.println("Opening water pump to drain water...");
  digitalWrite(WATER_PUMP_PIN, HIGH); // Turn on water pump
  
  // Monitor water level until it reaches 50% (10cm distance from sensor)
  while (true) {
    waterLevel = readUltrasonicDistance();
    Serial.print("Current distance to water: ");
    Serial.print(waterLevel);
    Serial.println(" cm");
    
    // When distance reaches 10cm, water level is at 50%
    if (waterLevel >= HALF_WATER_LEVEL) {
      break; // Reached 50% level (water drained enough)
    }
    
    // Safety check - if distance becomes too large, stop pump
    if (waterLevel > 20) {
      Serial.println("⚠️ Safety stop - water level too low!");
      break;
    }
    
    delay(500); // Check every 500ms
  }
  
  digitalWrite(WATER_PUMP_PIN, LOW); // Turn off water pump
  Serial.println("Water pump closed - 50% water removed");
  delay(500);
}

void fillWaterTo100Percent() {
  Serial.println("Opening solenoid valve to fill fresh water...");
  digitalWrite(SOLENOID_VALVE_PIN, HIGH); // Open solenoid valve
  
  // Monitor water level until it reaches 100% (5cm distance from sensor)
  while (true) {
    waterLevel = readUltrasonicDistance();
    Serial.print("Current distance to water: ");
    Serial.print(waterLevel);
    Serial.println(" cm");
    
    // When distance reaches 5cm, water level is at 100%
    if (waterLevel <= FULL_WATER_LEVEL) {
      break; // Reached 100% level (tank full)
    }
    
    // Safety check - if filling takes too long, stop valve
    static unsigned long fillStartTime = millis();
    if (millis() - fillStartTime > 60000) { // 1 minute timeout
      Serial.println("⚠️ Fill timeout - stopping valve!");
      break;
    }
    
    delay(500); // Check every 500ms
  }
  
  digitalWrite(SOLENOID_VALVE_PIN, LOW); // Close solenoid valve
  Serial.println("Solenoid valve closed - Tank filled to 100%");
  delay(500);
}

// Safety function to manually stop all actuators
void emergencyStop() {
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(SOLENOID_VALVE_PIN, LOW);
  cleaningInProgress = false;
  Serial.println("🛑 EMERGENCY STOP - All actuators turned OFF");
}