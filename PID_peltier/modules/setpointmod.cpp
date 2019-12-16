#include "setpointmod.h"

// Constants for min/max temperature
#define MAX_TEMP 50
#define MIN_TEMP 10

void SetPointMod::setup() {
}

/**
 * Register the handlers with the API base module.
 **/
void SetPointMod::serverSetup() {
  api.setup("/api/setpoint", true, true);
  name = "setpoint";
}

void SetPointMod::loop() {
}

/**
 * Info string about this module which is rendered on the root info page.
 **/
String SetPointMod::info() {
  return "<p>Allows for retrieving and setting of the PID set-point via the <a href=\"/api/setpoint\">/api/setpoint</a> endpoint.</p>";
}

/**
 * Returns the current value of the variable `setPoint` via the HTTP interface.
 **/
bool SetPointMod::getHandler(const char *path, JsonObject &reply) {
  reply["setpoint"] = setPoint;
  return true;
}

/**
 * Recevies a new value for the variable `setPoint` via the HTTP interface and
 * updates its value. The data is expected as a JSON data structure of the
 * format `{ "setpoint": n }`, where `n` is an integer between `MIN_TEMP` and
 * `MAX_TEMP`.
 **/
bool SetPointMod::putHandler(const char *path, const JsonVariant &request, JsonObject &reply) {
  // Make sure the argument has the right key
  if (request["setpoint"].success()) {
    int value = request["setpoint"].as<int>();

    // Make sure the value is within range
    if (value >= MIN_TEMP && value <= MAX_TEMP) {
      setPoint = value;
      return true;
    }
  }

  return false;
}
