int frequency = 50; //Specified in Hz
int buzzer_pin = D7; 

void setup()
{
  pinMode(buzzer_pin,OUTPUT); // Making Buzzer pin as output
}
void loop(){

tone(buzzer_pin, frequency);
delay(100);
noTone(buzzer_pin);
delay(100);

}
