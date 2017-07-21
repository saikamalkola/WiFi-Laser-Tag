/*
  Blitzkreig with wifi integration using NODEMCU(ESP8266) module.

  IR PROTOCOL 16bits:
  [Start_bit][15:0][end_bit]
  Data_Format----->[15:0] binary coded
  Example data:
  0011             0001               00101011(0 to 255)
   I               II                 III
  I  --->
         GOD_GUN ID(0)
         Player ID(1-12)
         Medic and ammo Kit (13)
         MINES  (14)
         GRENADES (15)
  II --->
         GOD_GUN functions:(I = 0000)
         0000  ----> Kill player ---> Player HP = 0
         0001  ----> Pause player ---> No activity up to 60 seconds.
         0010  ----> Full Ammo   ---> Ammo = maxAmmo
         0011  ----> Test sensors ---> Set player in test RX mode
         0100  ----> Test IR led ---> set gun so that it emits its pulse every second
         0101  ----> Penalty   ---> HP = HP - 10; Ammo = Ammo -5
         0110  ----> Award --->  HP = HP + 10 ; Ammp = Ammo + 5
         0111  ----> Clear log ---> clears EEPROM data of player
         1000  ----> Initialise player ---> Reset player for new game
         1001  ----> All leds test
         1011to 1111 ----> Undefined or do Nothing

         Player Functions:(I = 0001 to 1100)

         II --> xx yy
         xx tells the type of weapon
         yy tells game mode

         We can have 4 possible weapons.


        xxyy
        0001 ---> hand_gun(1)
        0110 ---> shot_gun(6)
        1010 ---> AK_47(10)
        1111 ---> Sniper(15)

        Other Weapons Functions:
        Medic and ammo kit:(I = 13):
        II ---> HP = HP + (HP_factor*value of II)
                Ammo = Ammo + (Ammo_factor*value of II)
        Mines and Grenades (I = 14 and 15)
        II ---> HP = HP - (HP_factor*value of II)

  III --->
         Last three digits of IP address of the server running on NODEMCU
         192.168.1.x
         III is the value of x
  Code by:
  K S S M KAMAL
  NITK SURATHKAL
  Dept of ECE
*/
//WiFi Libraries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//IR libraryfor ESP8266
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

//EEPROM library
#include <EEPROM.h>

//i2c library and OLED library for controlling ESP8266
#include <Wire.h>
#include "SSD1306.h"

const char* ssid = "kamal";       //my WiFi or Hotspot name
const char* password = "kamalkamal";//my WiFi Password

SSD1306  display(0x3c, D3, D5);

//Pin declarations
int rx_pin  = D2;   //38khz TSOP Receiver
int tx_pin = D6;    //Infra Red Led----transmits coded IR pulses
int trigger_pin = D4;  //Pushbutton (pulled-up) acts as trigger for gun
int buzzer_pin = D7; //buzzer to indicate firing and receiving of signal

IRrecv irrecv(rx_pin);
decode_results results;

IRsend irsend(tx_pin);

//player details
int player_ID = 8;//total of 12 players
int game_mode = 0;//0-Team mode and 1-Singlemode
int team_ID = player_ID / 7;
int score = 0; //gets incremented when player makes a hit

//variables
int max_hp = 100;
int max_ammo = 20;
int hp = max_hp;        //hp indicates life of player at current time
int ammo = max_ammo;    //ammo indicates player bullets at current time
int last_hp = hp - 1;
int last_ammo = ammo - 1;
boolean control_fire = 1;
//if control_fire = 1 then the gun sends pulses only once when button is pressed
//if control_fire = 0 then the gun sends pulses as long as butto is pressed...continuous fire :)
boolean lastbutton_state = HIGH;
boolean button_state = HIGH;
boolean new_game = true; //To reset EEPROM data and start a new game
int hit_damage = 5;  //hp will reduce by this amount when got hit
int EEPROM_address = 0;  //EEPROM base address
uint8_t hit_IP = 0;   //IP address of player who made hit
uint16_t timeout = 5000;


//blitzkreig website variables
unsigned long int update_db = 0;
int update_db_interval = 10000; //update database of website every 1000 milliseconds
String player_name = "Test";
int reset = 0, prev_reset = 0;
char server[100] = "http://www.saikamalkola.com/";//Server address
int port = 80; //default port
String response;

//session variables
int time_minutes = 5; //Game wil get reset after 5 minutes...i.e., session over
unsigned long time_limit = time_minutes * 60 * 1000; //converting session time into milliseconds
unsigned long present_ms = 0, time_ms = 0;

HTTPClient http;//http client object to communicate with website

void setup()
{
  //Serial.begin(115200);
  //connecting to WiFi or Hotspot
  WiFi.begin(ssid, password);
  //Serial.println("");

  // Waiting for connection
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  present_ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    if(millis() - present_ms > timeout)
    {
      break;
    }
  }
  //connection successfull
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());  //IP address assigned to your ESP by my mobile/router

  //EEPROM
  EEPROM.begin(4);
  hp = EEPROM.read(0);
  ammo = EEPROM.read(1);
  //assigning 4 bytes of memory in EEPROM

  //IR rx and tx
  irsend.begin();
  irrecv.enableIRIn(); // Start the receiver
  pinMode(trigger_pin, INPUT_PULLUP);//declaring pushbutton as input and enabling internal Pullup
  pinMode(buzzer_pin, OUTPUT);  //making buzzer pin as output

  //Setting up OLED display I2C protocol.
  //SCL--->D5
  //SDA--->D3
  //initialising display
  req_server();  //fetch player details
  display.init();
  display.flipScreenVertically();
  display_hp_ammo();
  time_ms = millis(); //taking the elapsed milliseconds in to account :)
}


void loop()
{
  present_ms = millis();
  if (reset != 0 && prev_reset == 0)
  {
    new_game = true;
    time_ms = present_ms;
  }
  else
  {
    new_game = false;
  }

  prev_reset = reset;
  if (new_game == true)
  {
    //reset hp and ammo to their previous values
    hp = max_hp;
    ammo = max_ammo;
    new_game = false;
    update_EE();
  }
  else
  {
    hp = EEPROM.read(0);
    ammo = EEPROM.read(1);
  }
  if (present_ms - time_ms < time_limit)
  {
    if (present_ms - update_db > update_db_interval)
    {
      display_hp_ammo();
      req_server();
      update_db = present_ms;
    }

    tx_rx_check();//Checking for trigger press(tx) and IR signals(rx)

    if (last_hp != hp || last_ammo != ammo && hp > 0 && ammo > 0 )
    {
      display_hp_ammo();
      last_hp = hp;
      last_ammo = ammo;
    }
  }
  else
  {
    display_game_over();
    req_server();
    delay(100);
  }
}

void update_EE()
{
  EEPROM.write(0, hp);
  EEPROM.commit();
  EEPROM.write(1, ammo);
  EEPROM.commit();
}

void player_dead()
{
  //Serial.println("player dead!!!");
  //when life of player becomes zero..making some sounds
  tone(buzzer_pin, 50);
  //updating OLED display
  display_player_dead();
  delay(1000);
  noTone(buzzer_pin);
  delay(1000);
}
void no_ammo()
{
  //Serial.println("Ammo Over!!");
  tone(buzzer_pin, 50);
  //Updating OLED display
  display_ammo_over();
  delay(100);
  noTone(buzzer_pin);
  delay(1000);
}
void player_hit()
{
  if (hp <= 0)
  {
    player_dead();
  }
  else
  {
    hp = hp - hit_damage;
    update_EE();
  }
}

void decode_rx_data(uint16_t data)
{
  player_hit();
}

void got_hit(uint16_t data)
{
  tone(buzzer_pin, 50);
  delay(50);
  noTone(buzzer_pin);
  decode_rx_data(data);
}

void tx_rx_check()
{
  // If button pressed, send the code.
  button_state = digitalRead(trigger_pin);
  if (lastbutton_state == LOW && button_state == HIGH) {
    //Serial.println("Trigger Released");
    irrecv.enableIRIn(); // Re-enable receiver
  }
  if (control_fire == 1)
  {
    if (button_state == LOW && lastbutton_state == HIGH)
    {
      delay(5); //debounce the button
      button_state = digitalRead(trigger_pin);
      if (button_state == LOW && lastbutton_state == HIGH)
      {
        //trigger pressed
        if (ammo > 0 && hp > 0)
        {
          ammo--;
          update_EE();
          irsend.sendNEC(0x7100, 16);
          //Serial.println("Pressed, sending");
          //Serial.println(String(0x1100, HEX));
          delay(50); // Wait a bit between retransmissions
        }
        else if (ammo <= 0)
        {
          no_ammo();
        }
        else
        {
          player_dead();
        }
      }
    }
    else if (irrecv.decode(&results)) {
      if ((uint16_t)results.value == 0x1100)
      {
        //Serial.println((uint16_t)results.value, HEX);
        got_hit((uint16_t)results.value);
      }
      irrecv.resume(); // resume receiver
    }
    lastbutton_state = button_state;
  }
  else
  {
    if (button_state == LOW ) {
      delay(5); //debounce the button
      button_state = digitalRead(trigger_pin);
      if (button_state == LOW )
      {
        //trigger pressed
        ammo--;
        //Serial.println("Pressed, sending");
        irsend.sendNEC(0x1100, 16);
        delay(50); // Wait a bit between retransmissions
      }
    }
    else if (irrecv.decode(&results)) {
      if (results.value == 0x7100)
      {
        //Serial.println((uint16_t)results.value, HEX);
        got_hit(results.value);
      }
      irrecv.resume(); // resume receiver
    }
    lastbutton_state = button_state;
  }
}

void display_hp_ammo()
{

  int disp_hp = map(hp, 0, max_hp, 0, 100);
  int disp_ammo = map(ammo, 0, max_ammo, 0, 100);
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Name: " + String(player_name));
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 22, "hp:" + String(hp));
  display.drawProgressBar(0, 34, 100, 8, disp_hp);
  display.drawString(0, 44, "Ammo:" + String(ammo));
  display.drawProgressBar(0, 55 , 100, 8, disp_ammo);
  display.display();
}

void display_got_hit()
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 10, "GOT HIT!!");
  display.setFont(ArialMT_Plain_10);
  display.drawString(30, 30, "PLAYER ID: ");
  //display.drawString(40,45,String(results.value,HEX));
  display.display();
  delay(1000);
}

void display_player_dead()
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "health: " + String(hp));
  display.setFont(ArialMT_Plain_10);
  display.drawString(40, 40, "PLAYER DEAD!!!");
  display.display();
}

void display_game_over()
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Time Up!" );
  display.setFont(ArialMT_Plain_10);
  display.drawString(40, 40, "Game Over!!");
  display.display();
}

void display_ammo_over()
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "AMMO: " + String(ammo));
  display.setFont(ArialMT_Plain_10);
  display.drawString(40, 40, "AMMO OVER!!!");
  display.display();
}

void made_hit()
{
  display.clear();
  display.drawString(0, 10, "Score: " + String(score));
  display.setFont(ArialMT_Plain_10);
  display.drawString(40, 40, "Made hit!");
  display.display();
}
void parse_response()
{
  int l = response.length(), k = 0;
  int limits[100];
  for (int i = 0; i < l - 1 ; i++)
  {
    if (response[i] == '#' && response[i + 1] == '_')
    {
      limits[k] = i + 2;
      k++;
    }
  }
  player_name = response.substring(limits[0], limits[1] - 2);
  String temp = response.substring(limits[1], limits[2] - 2);
  reset = temp.toInt();
}
void req_server()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    String url = "LaserTag/get_player_data.php/?player=";
    url += String(player_ID);
    url += "&hp=";
    url += String(hp);
    url += "&ammo=";
    url += String(ammo);
    url += "&time=";
    url += String(present_ms - time_ms);

    String request = server + url;
    http.begin(request);
    int response_code = http.GET();
    if (response_code == HTTP_CODE_OK)
    {
      response = http.getString();
      //Serial.println(response);
    }
    else {
      //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(response_code).c_str());
    }
    http.end();
    delay(10);
    parse_response();
  }
}
