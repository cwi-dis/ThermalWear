#ifndef setpointmod_h
#define setpointmod_h

#include <iotsa.h>
#include <iotsaApi.h>

class SetPointMod : public IotsaApiMod {
public:
  SetPointMod(IotsaApplication &app, double &setPoint) : IotsaApiMod(app), setPoint(setPoint) {};
  void setup();
  void serverSetup();
  void loop();
  String info();

protected:
  bool getHandler(const char*, JsonObject&);
  bool putHandler(const char*, const JsonVariant&, JsonObject&);

private:
  double &setPoint;
};

#endif
