#include <Wire.h>
#include "SSD1306.h"

SSD1306  display(0x3c, D3, D5);

int max_hp = 100;
int max_ammo = 20;
int hp = max_hp;
int ammo = max_ammo;
int last_hp = hp-1;
int last_ammo = ammo-1;
boolean test_disp = 1;

void setup()
{
  //Setting up OLED display I2C protocol.
  //SCL--->D5
  //SDA--->D3
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display_hp_ammo();
}

void loop()
{
  if(test_disp == 1)
  {
    test_display();
    test_disp = 0;
  }
 if(ammo == 0)
 {
//  no_ammo();
 }
  if(hp == 0)
 {
 // player_dead();
 }

 if(last_hp != hp || last_ammo != ammo && hp != 0 && ammo != 0)
 {
  display_hp_ammo();
  last_hp = hp;
  last_ammo = ammo;
 }
 delay(500); 
}

void display_hp_ammo()
{ 

  int disp_hp = map(hp,0,max_hp,0,100);
  int disp_ammo = map(ammo,0,max_ammo,0,100);
  Serial.println("hp:"+String(disp_hp)+"ammo:"+String(disp_ammo));
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 0, "TECHNITES");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "hp:"+String(hp));
  display.drawProgressBar(0, 32, 100, 8, disp_hp);
  display.drawString(0, 42, "ammo:"+String(ammo));
  display.drawProgressBar(0,54 , 100, 8,disp_ammo);
  display.display();
}

void display_got_hit()
{
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(10,10,"GOT HIT!!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(30,30,"PLAYER ID: ");
    //display.drawString(40,45,String(results.value,HEX));
    display.display();
    delay(1000);
}

void display_player_dead()
{

    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10,"health: "+String(hp));
    display.setFont(ArialMT_Plain_10);
    display.drawString(40,40,"PLAYER DEAD!!!");
    display.display();
}

void display_ammo_over()
{
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0,10,"AMMO: "+String(ammo));
    display.setFont(ArialMT_Plain_10);
    display.drawString(40,40,"AMMO OVER!!!");
    display.display();
}

void test_display()
{
 display_hp_ammo();
 delay(1000);
 display_ammo_over();
 delay(1000);
 display_player_dead();
 delay(1000);
 display_got_hit();
 delay(1000);
}

