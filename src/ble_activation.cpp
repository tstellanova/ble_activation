
#include "Particle.h"

// the mode affects how the system handles connection with no prior wifi creds

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

static bool _creds_collected = false;
static uint8_t _mac_address[6] = {};

SerialLogHandler logHandler(LOG_LEVEL_ALL);


void setup() {
    pinMode(D7, OUTPUT);
    digitalWrite(D7, HIGH);  
    delay(4000); //wait for usb serial wakeup
    
    WiFi.macAddress(_mac_address);
    Log.info("MAC: %02x:%02x:%02x:%02x:%02x:%02x", 
    _mac_address[0],_mac_address[1],_mac_address[2],
    _mac_address[3],_mac_address[4],_mac_address[5]);

    // this sample force-clears any wifi credentials on the device:
    // ordinarily you'd maybe check to see whether you need to get credentials
    // and then continue
    if (!_creds_collected) {
      if (WiFi.hasCredentials()) {
        Log.info("hasCredentials");
        //force clear credentials
        WiFi.clearCredentials();
        delay(500);
        Log.info("clearCredentials");
      }

      BLE.setDeviceName("FunkyActivator");
      Log.info("start listening...");
      WiFi.listen();
      String actual_name = BLE.getDeviceName();
      Log.info("listening with name: %s", actual_name.c_str());
    }
}

// WiFi.listen() plays various games with the user app thread-- see
// the Particle SDK docs for more details. The behavior is different
// when system thread is enabled. 
void loop() {
  static uint16_t loop_count = 0;
  Log.info("loop %d", loop_count);

  if (!_creds_collected) {
    if (WiFi.listening()) {
      // Log.info("still listening...");
      delay(100);
    }
    else if (WiFi.hasCredentials()) {
      _creds_collected = true;
      String actual_name = BLE.getDeviceName();
      Log.info("%s creds collected after %d", actual_name.c_str(), loop_count);
      WiFi.listen(false);
    }
    else {
      // Log.info("no creds yet...");
      // delay(1000);
    }
  }
  else {
    // we must trigger a connect ourselves if we're not in AUTOMATIC mode
    if (!Particle.connected()) {
      Particle.connect();
      bool result = waitFor(Particle.connected,30000);
      Log.info("connect result: %d", result);
    }
    else {
      Log.info("idle");
      delay(3000);
    }
  }

  loop_count++;

}
