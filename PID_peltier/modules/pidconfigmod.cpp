#include "pidconfigmod.h"

/**
 * Load the values from the config file during module setup
 **/
void PIDConfigMod::setup() {
  configLoad();
}

/**
 * Register the handlers with the API base module.
 **/
void PIDConfigMod::serverSetup() {
  api.setup("/api/pidconfig", true, true);
  name = "pidconfig";
}

void PIDConfigMod::loop() {
}

/**
 * Info string about this module which is rendered on the root info page.
 **/
String PIDConfigMod::info() {
  return "<p>Allows for storing and retrieving of the PID parameters inside a config file via the endpoint <a href=\"/api/pidconfig\">/api/pidconfig</a>.</p>";
}

/**
 * Retrieves the PID config from local variables and returns them in a JSON
 * object.
 **/
bool PIDConfigMod::getHandler(const char *path, JsonObject &reply) {
  reply["p"] = KP;
  reply["i"] = KI;
  reply["d"] = KD;

  return true;
}

/**
 * Takes a JSON object and updates the local variables for the PID if either of
 * the keys `p`, `i` or `d` are present. The values for these keys need to be
 * numeric. After updating the local variables, `configSave()` is called to
 * commit the changes to storage. Note that this does not affect the PID
 * immediately. The device should be reset in order for * the changes to
 * actually have an effect on the PID.
 **/
bool PIDConfigMod::putHandler(const char *path, const JsonVariant &request, JsonObject &reply) {
  IotsaConfigFileSave cf("/config/pid.cfg");

  // Check if the key `p` is present and update the variable `KP`
  if (request["p"].success()) {
    KP = request["p"].as<int>();
    Serial.print("Saved new value for P ");
    Serial.println(KP);
  }

  // Check if the key `i` is present and update the variable `KI`
  if (request["i"].success()) {
    KI = request["i"].as<int>();
    Serial.print("Stored new value for I ");
    Serial.println(KI);
  }

  // Check if the key `d` is present and update the variable `KD`
  if (request["d"].success()) {
    KD = request["d"].as<int>();
    Serial.print("Stored new value for D ");
    Serial.println(KD);
  }

  // Store new values in the config file
  configSave();
  return true;
}

/**
 * Tries to load the keys `p`, `i` and `d` from the config and updates the
 * variables `KP`, `KI` and `KD`. Note that this should be done before
 * initialising the PID for the values to have an effect and circumvent
 * undefined behaviour.
 **/
void PIDConfigMod::configLoad() {
  IotsaConfigFileLoad cf("/config/pid.cfg");

  // Retrieve the values from the config file and store them in the variables
  cf.get("p", KP, KP);
  cf.get("i", KI, KI);
  cf.get("d", KD, KP);

  // Print the loaded values
  Serial.print("Loaded PID params from config file:");
  Serial.print(" P=");
  Serial.print(KP);
  Serial.print(" I=");
  Serial.print(KI);
  Serial.print(" D=");
  Serial.println(KD);
}

/**
 * Commits the values of the variables `KP`, `KI` and `KD` to the config.
 **/
void PIDConfigMod::configSave() {
  IotsaConfigFileSave cf("/config/pid.cfg");

  // Save values of local variables in config file
  cf.put("p", KP);
  cf.put("i", KI);
  cf.put("d", KD);
}
