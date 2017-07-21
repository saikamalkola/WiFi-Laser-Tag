#include <ESP8266HTTPClient.h>

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
#include <ESP8266WebServer.h>


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

ESP8266WebServer server(80); //Server on port 80

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
int player_ID = 1;//total of 12 players
uint8_t player_IP = 0;//last three letters of player's ESP server IP address
int game_mode = 0;//0-Single mode and 1-Team mode
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
boolean new_game = false; //To reset EEPROM data and start a new game
int hit_damage = 5;  //hp will reduce by this amount when got hit
int EEPROM_address = 0;  //EEPROM base address
uint8_t hit_IP = 0;   //IP address of player who made hit

//blitzkreig website variables
unsigned long int update_db = millis();
int update_db_interval = 1000; //update database of website every 1000 milliseconds

//websever variables
unsigned long int update_server = millis();
int update_server_interval = 1000; //update local server every 1000 milliseconds

//Starting the web server
String html_page = ""; //entire HTML code stored as a String

void handleRoot() {
  html_page = "<HTML> <HEAD> <TITLE>Blitzkreig</TITLE> <meta http-equiv=\"refresh\" content=\"5\" >  </HEAD> <style type=\"text/css\"> .center{ text-align: center; } .head-color{ color : #092040; } </style> <BODY> <h1 class = \"center head-color\"><strong>TECHNITES - Blitzkreig</strong></h1><br> <br> <br> <br> <br> <form action = \"reset\" class=\"center\" method=\"GET\"> <p>Set true and press reset button to reset player and clear EEPROM</p> <input type=\"text\" name=\"reset\"/> <input type=\"submit\" value=\"reset\"/> <br> <br> <h2>Player Stats with real time update</h2> Hit Points : ";
  html_page += hp;
  html_page += "<br>ammo: ";
  html_page += ammo;
  html_page += " </form> </body> </html>";

  //Read HTML contents
  server.send(200, "text/html", html_page); //Send web page
}


void setup()
{
  Serial.begin(115200);
  yield();
  //connecting to WiFi or Hotspot

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Waiting for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //connection successfull
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP by my mobile/router

  player_IP = parse_IP(WiFi.localIP().toString());
  Serial.println("player_IP: " + String(player_IP));

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/reset", reset_player);
  server.on("/madeHit", made_hit);
  server.begin();                  //Start server
  Serial.println("HTTP server started");

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
  display.init();
  display.flipScreenVertically();
  display_hp_ammo();
}


void loop()
{
  server.handleClient(); //server handles connected devices or clients

  if (new_game == true)
  {
    //reset hp and ammo to their previous values
    hp = max_hp;
    ammo = max_ammo;
    new_game == false;
    update_EE();
  }
  else
  {
    hp = EEPROM.read(0);
    ammo = EEPROM.read(1);
  }
  Serial.println("reset:" + String(new_game) + " " + String(hp) + " " + String(ammo));
  tx_rx_check();
  if (last_hp != hp || last_ammo != ammo && hp > 0 && ammo > 0 )
  {
    display_hp_ammo();
    last_hp = hp;
    last_ammo = ammo;
  }
  new_game == false;
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
  Serial.println("player dead!!!");
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
  Serial.println("Ammo Over!!");
  tone(buzzer_pin, 50);
  //Updating OLED display
  display_ammo_over();
  delay(100);
  noTone(buzzer_pin);
  delay(1000);
}
void player_hit(int test)
{
  if (hp <= 0)
  {
    player_dead();
  }
  else
  {
    hp = hp - hit_damage;
    update_EE();
    feedback_hit_server();
  }

}

void feedback_hit_server()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient hitServer;

    Serial.print("[HTTP] begin...\n");

    //storing
    hitServer.begin("192.168.43." + String(hit_IP)+"/madeHit?madeHit=" + String(player_IP)); //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = hitServer.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = hitServer.getString();
        Serial.println(payload);
      }
    } 
    
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", hitServer.errorToString(httpCode).c_str());
    }
    //hitServer.end();
  }

  /* WiFiClient hitServer;
    String host = "192.168.43." + String(hit_IP);
    const int port = 80;
    Serial.println("connecting to"+host+":"+port);
    if (!hitServer.connect(host.c_str(), port))
    {
     Serial.println("connection failed");
     return;
    }
    String url = "/madeHit?madeHit=" + String(player_IP);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    hitServer.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    // Read all the lines of the reply from server and print them to Serial
    while(hitServer.available()){
     String line = hitServer.readStringUntil('\r');
     Serial.print(line);
    }*/
}
void decode_rx_data(uint16_t data)
{
  uint16_t hit_IP_buffer = 255;
  hit_IP = data & hit_IP_buffer;
  uint16_t player_data_buffer = 65280;
  int test = data & player_data_buffer;
  Serial.println("hit_ip:" + String(hit_IP) + "hit player:" + String(test));
  player_hit(test);

}

void got_hit(unsigned long data)
{
  tone(buzzer_pin, 50);
  delay(200);
  noTone(buzzer_pin);
  decode_rx_data(data);
}

void tx_rx_check()
{
  // If button pressed, send the code.
  button_state = digitalRead(trigger_pin);
  if (lastbutton_state == LOW && button_state == HIGH) {
    Serial.println("Trigger Released");
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
          irsend.sendNEC(0x7100 | player_IP, 16);
          Serial.println("Pressed, sending");
          Serial.println(String(0x7100 | player_IP, HEX));
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
      if ((uint16_t)results.value == 0x117A)
      {
        Serial.println((uint16_t)results.value, HEX);
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
        Serial.println("Pressed, sending");
        irsend.sendNEC(0x1100, 16);
        delay(50); // Wait a bit between retransmissions
      }
    }
    else if (irrecv.decode(&results)) {
      if (results.value == 0x717A)
      {
        Serial.println((uint16_t)results.value, HEX);
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
  display.drawString(10, 0, "TECHNITES");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "hp:" + String(hp));
  display.drawProgressBar(0, 32, 100, 8, disp_hp);
  display.drawString(0, 42, "ammo:" + String(ammo));
  display.drawProgressBar(0, 54 , 100, 8, disp_ammo);
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
  String got_hit_player_IP = server.arg("madeHit");
  server.send(200, "text/plain", "1");
  score++;
  display.clear();
  display.drawString(0, 10, "Score: " + String(score));
  display.setFont(ArialMT_Plain_10);
  display.drawString(40, 40, "Made hit!");
  display.display();
}
int parse_IP(String ip)
{
  int IP_val = 0;
  IP_val = (ip.substring(ip.lastIndexOf('.') + 1)).toInt();
  return IP_val;
}

void reset_player()
{
  String New = server.arg("reset");
  if (New == "true")
  {
    new_game = true;
  }
  else
    new_game = false;
  handleRoot();
  Serial.println(New);
}
