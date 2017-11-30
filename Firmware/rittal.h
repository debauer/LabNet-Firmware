#include "setup.h"            // project folder

#include "pinning.h"          // project folder
#include "adressen.h"         // project folder
#include "defines.h"          // project folder
#include "structs.h"	// project folder
#include "globals.h"          // Globale Variablen.

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

class Rittal {
  public:
    Rittal();
    
    void init();
    void task();
    void setPlug(uint8_t id, uint8_t plug);
    void setLeiste(uint8_t id, uint8_t value);
    void setSocket(uint8_t id, uint8_t socket, uint8_t value);
 	  void setAvail(uint8_t id, uint8_t value);
    void setMin(uint8_t id, uint8_t value);
    void setMax(uint8_t id, uint8_t value);
    void resetAll();
    void reset(uint8_t id);
    rittal_s leiste[4];

  private:
  	void sendData(rittal_s r);
  	void sendReq(uint8_t id);
    void saveReqAnswer();
    void sendDebug(char arr[]);
    void retrySwitch();
    void sendRittalAnnounce(uint8_t rid);
};
