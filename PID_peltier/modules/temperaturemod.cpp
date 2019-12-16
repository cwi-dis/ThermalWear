#include "temperaturemod.h"

void TemperatureMod::setup() {
}

/**
 * Register the handlers with the API base module.
 **/
void TemperatureMod::serverSetup() {
  api.setup("/api/temperature", true);
  name = "temperature";
}

void TemperatureMod::loop() {
}

/**
 * Info string about this module which is rendered on the root info page.
 **/
String TemperatureMod::info() {
  return "<p>Returns the current temperature of the Peltier element via the <a href=\"/api/temperature\">/api/temperature</a> endpoint.</p>";
}

/**
 * Returns the current value of the variable `temperature` via the HTTP interface.
 **/
bool TemperatureMod::getHandler(const char *path, JsonObject &reply) {
  reply["temperature"] = temperature;
  return true;
}
