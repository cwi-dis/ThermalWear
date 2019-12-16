#ifndef temperaturemod_h
#define temperaturemod_h

#include <iotsa.h>
#include <iotsaApi.h>

class TemperatureMod : public IotsaApiMod {
public:
  TemperatureMod(IotsaApplication &app, double &temperature) : IotsaApiMod(app), temperature(temperature) {};
  void setup();
  void serverSetup();
  void loop();
  String info();

protected:
  bool getHandler(const char*, JsonObject&);

private:
  double &temperature;
};

#endif
