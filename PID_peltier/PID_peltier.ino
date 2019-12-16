#include <PID_v1.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include <iotsa.h>
#include <iotsaWifi.h>
#include <iotsaOta.h>
#include <iotsaLed.h>

#include "modules/pidconfigmod.h"
#include "modules/setpointmod.h"
#include "modules/temperaturemod.h"

// Constants for pins
#define PID_OUTPUT 13
#define PID_COOL 4
#define PID_WARM 5
#define TEMP_PROBE 14

#define LED_OUTPUT 15

// Delay for reading the digital temp sensor in ms
#define TEMP_READ_DELAY 20

// PID settings
int KP = 200;
int KI = 2;
int KD = 0;

// Initialise iotsa
IotsaApplication app("Iotsa PID controller");
IotsaWifiMod wifiMod(app);
IotsaOtaMod otaMod(app);
IotsaLedMod ledMod(app, LED_OUTPUT);

// Initialise PID
double temperature, setPoint, outputVal;
PID *myPID;

// Initialise temperature prove
OneWire oneWire(TEMP_PROBE);
DallasTemperature temperatureSensors(&oneWire);

// Track time of last clock update
unsigned long lastTempUpdate, lastDebugUpdate;

// Instantiate module and install it in the application
SetPointMod setPointMod(app, setPoint);

// Initialise the config mod and register it with the application
PIDConfigMod configMod(app, KP, KI, KD);

// Initialise the temperature mod and register it with the application
TemperatureMod temperatureMod(app, temperature);

/**
 * Updates the global variable `temperature` in a interval given by the
 * constant `TEMP_READ_DELAY`. Returns true if the variable was updated, false
 * otherwise.
 **/
bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    // Get temp reading
    temperatureSensors.requestTemperatures();
    temperature = temperatureSensors.getTempCByIndex(0);

    lastTempUpdate = millis();

    return true;
  }

  return false;
}

/**
 * Initialises the output pins, the serial connection, the initial `setPoint`,
 * the temperature sensor and sets up initial values for the PID.
 **/
void setup() {
  Serial.begin(9600);

  // Initialise output pins
  pinMode(PID_OUTPUT, OUTPUT);
  pinMode(PID_COOL, OUTPUT);
  pinMode(PID_WARM, OUTPUT);

  // Set up iotsa application
  app.setup();
  app.serverSetup();

  // Initialise temperature sensors
  temperatureSensors.begin();
  temperatureSensors.requestTemperatures();

  // Wait until we got a reading from the temperature sensors
  while (!updateTemperature()) {}

  // Print values that are used to initialise PID
  Serial.print("Initialising PID with values: ");
  Serial.print(KP);
  Serial.print(",");
  Serial.print(KI);
  Serial.print(",");
  Serial.println(KD);

  // Initialise initial target temperature to 32 degrees
  setPoint = 32;

  // Initialise the PID
  myPID = new PID(&temperature, &outputVal, &setPoint, KP, KI, KD, DIRECT);
  myPID->SetOutputLimits(-1023, 1023);
  myPID->SetMode(AUTOMATIC);
}

/**
 * Runs repeatedly after initialisation. Reads current temperature from the
 * temperature sensor, waits for command input from the serial port to update
 * the variable `setPoint` and calculates values for adjusting the PID and thus
 * the temperature of the Peltier element. Prints debug output after each
 * iteration.
 **/
void loop() {
  // Main loop for iotsa application
  app.loop();

  // Update temperature reading every so often
  updateTemperature();

  // Set tuning params for PID in case they have changed
  myPID->SetTunings(KP, KI, KD);

  // Compute output value for PID based on `temperature` and `setPoint`
  myPID->Compute();

  if (setPoint == 38.00) {
    KP = 1000;

    if (temperature <= 35.00) {
      outputVal = 1023;
    } else if (temperature > 35.00 && temperature < 37.75) {
      outputVal = 300;
    } else if (temperature >= 37.75 && temperature <= 38) {
      outputVal = 0;
    }
  } else if (setPoint == 28.00) {
    KP = 400;

    if (temperature >= 28.75) {
      outputVal = -1023;
    } else if (temperature < 28.75 && temperature >= 28.25) {
      outputVal = -900;
    } else if (temperature < 28.25 && temperature >= 28) {
      outputVal = 0;
    }
  } else if (setPoint == 32.00) {
    if (temperature > 33.00 || temperature < 31.00) {
      KP = 165;
    } else {
      KP = 145;
    }
  }

  // Start heating if calculated `outputVal` is greater than or equal to zero
  if (outputVal >= 0) {
    // heating
    digitalWrite(PID_COOL, LOW);
    digitalWrite(PID_WARM, HIGH);

    if (outputVal > 70 && outputVal < 250) {
      outputVal = 250;
    }

    if (outputVal >= 0 && outputVal <= 70) {
      outputVal = 0;
    }
  } else {
    // cooling
    digitalWrite(PID_COOL, HIGH);
    digitalWrite(PID_WARM, LOW);

    if (outputVal < -70 && outputVal > -250) {
      outputVal = -250;
    }

    if (outputVal >= -70 && outputVal < 0) {
      outputVal = 0;
    }
  }

  if (temperature > 44.00) {
    outputVal = 0;
  }

  // Write output value to PID output
  analogWrite(PID_OUTPUT, int(abs(outputVal)));

  double gap = setPoint - temperature;

  if ((millis() - lastDebugUpdate) > TEMP_READ_DELAY) {

    // Debug output
    Serial.print("Temperature is: ");
    Serial.print(temperature);
    Serial.print(" setpoint ");
    Serial.print(setPoint);
    Serial.print(" gap ");
    Serial.print(gap);
    Serial.print(" outputVal ");
    Serial.print(outputVal);
    Serial.print(" PID ");
    Serial.print(KP);
    Serial.print(",");
    Serial.print(KI);
    Serial.print(",");
    Serial.println(KD);

    lastDebugUpdate = millis();
  }
}
