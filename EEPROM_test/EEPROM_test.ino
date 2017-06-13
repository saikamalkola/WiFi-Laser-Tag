#include <EEPROM.h>
int hp = 100;
void setup() {
  Serial.begin(9600);
    EEPROM.begin(4);
    hp = EEPROM.read(0);
}

void loop() {
  hp--;
  update_EE();
  hp = EEPROM.read(0);
  Serial.println(hp);
  delay(1000);
  }
 void update_EE()
 {
  EEPROM.write(0,hp);
  EEPROM.commit();
 }

