#ifndef pidconfigmod_h
#define pidconfigmod_h

#include <iotsaConfig.h>
#include <iotsaConfigFile.h>

class PIDConfigMod : public IotsaConfigMod {
public:
  PIDConfigMod(IotsaApplication &app, int &KP, int &KI, int &KD) : IotsaConfigMod(app), KP(KP), KI(KI), KD(KD) {}
  void setup();
  void serverSetup();
  void loop();
  String info();

protected:
  bool getHandler(const char*, JsonObject&);
  bool putHandler(const char*, const JsonVariant&, JsonObject&);

private:
  int &KP;
  int &KI;
  int &KD;

  void configLoad();
  void configSave();
};

#endif
