#include <IRremoteESP8266.h>

int rx_pin  = D2;   //38khz TSOP Receiver
int tx_pin = D6;    //Infra Red Led----transmits coded IR pulses
int trigger_pin = D4;  //Pushbutton (pulled-up) acts as trigger for gun
int buzzer_pin= D7;  //buzzer to indicate firing and receiving of signal


IRrecv irrecv(rx_pin);
decode_results results;

IRsend irsend(tx_pin);

boolean control_fire = 1;
boolean lastbutton_state = HIGH;
boolean button_state = HIGH;

void setup()
{
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn(); // Start the receiver
  pinMode(trigger_pin, INPUT_PULLUP);//declaring pushbutton as input and enabling internal Pullup
  pinMode(buzzer_pin, OUTPUT);  //making buzzer pin as output
}


void loop() {
  // If button pressed, send the code.
  button_state = digitalRead(trigger_pin);
  if (lastbutton_state == LOW && button_state == HIGH) {
    Serial.println("Trigger Released");
    irrecv.enableIRIn(); // Re-enable receiver
  }
if(control_fire == 1)
{
  if (button_state == LOW && lastbutton_state == HIGH) {
    delay(5); //debounce the button
    button_state = digitalRead(trigger_pin);
    if(button_state == LOW && lastbutton_state == HIGH)
    {
    Serial.println("Pressed, sending");
    tone(buzzer_pin,100);
    irsend.sendNEC(0x1100, 16);
    delay(50); // Wait a bit between retransmissions
    noTone(buzzer_pin);
    } 
  }
  else if (irrecv.decode(&results)) {
    if(results.value == 0x7100)
    {
    Serial.println(results.value,HEX);
    tone(buzzer_pin,50);
    delay(200);
    noTone(buzzer_pin);
    }
    irrecv.resume(); // resume receiver
  }
  lastbutton_state = button_state;
}
else
{
  if (button_state == LOW )
  {
    delay(5); //debounce the button
    button_state = digitalRead(trigger_pin);
    if(button_state == LOW)
    {
    Serial.println("Pressed, sending");
    tone(buzzer_pin,50);
    irsend.sendNEC(0x7100, 16);
    delay(50); // Wait a bit between retransmissions
    noTone(buzzer_pin);
    } 
  }
  else if (irrecv.decode(&results)) {
    if(results.value == 0x1100)
    {
    Serial.println(results.value,HEX);
    tone(buzzer_pin,500);
    delay(500);
    noTone(buzzer_pin);
    }
    irrecv.resume(); // resume receiver
  }
  lastbutton_state = button_state;
}
}
