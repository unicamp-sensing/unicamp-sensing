//code write by Moz for YouTube changel LogMaker360, 7-10-2015
//code belongs to this video: https://www.youtube.com/watch?v=FzJ3JqtQVko

int sensorPin = A0; // select the input pin for the potentiometer

int sensorValue = 0; // variable to store the value coming from the sensor
int counter;
void setup () 
{
  Serial.begin (9600);
}
 
void loop () 
{
  sensorValue = analogRead (sensorPin);
 
  delay (100);
  
  Serial.print (sensorValue, DEC);
  Serial.print(" ");
  counter++;
  if(counter==30){
    Serial.println();
    counter = 0;
  }
 }
