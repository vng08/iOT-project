#include "DHT.h"
#include "ESP32Servo.h"

const int dht_pin = 4;
const int mq135_pin = 32;
const int buzzer_pin = 5;
const int servo_air_conditioner_pin = 12;
const int servo_lamp_pin = 13;
const int pir_pin = 18;
const int photoresistor_pin = 33;

DHT dht(dht_pin, DHT11);
Servo servo_ac;
Servo servo_lamp;

float calculatePPM();

void setup() {

  // delay khoảng 10 giây để làm nóng cảm biến không khí
  delay(10000);

  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  pinMode(mq135_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);

  servo_ac.setPeriodHertz(50);
  servo_ac.attach(servo_air_conditioner_pin, 500, 2400);

  servo_lamp.setPeriodHertz(50);
  servo_lamp.attach(servo_lamp_pin, 500, 2400);

  pinMode(pir_pin, INPUT);
  pinMode(photoresistor_pin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Độ C
  float f = dht.readTemperature(true); // Độ F
  float ppm = calculatePPM();
  int pirValue = digitalRead(pir_pin);
  int photoresistorValue = analogRead(photoresistor_pin);

  if (ppm > 600)
  {
    tone(buzzer_pin, 1000);      // phát âm tần số Hz
  }
  else
  {
    noTone(buzzer_pin);
  }

  if (pirValue)
  {
    Serial.println("Có chuyển động");
  }
  Serial.println(photoresistorValue);

  /* Cách xoay servo
  servo_ac.write(180);
  servo_lamp.write(180);
  delay(500);
  servo_ac.write(0);
  servo_lamp.write(0);
  */
}

float calculatePPM()
{
   // Tìm ppm 
  float VRL; 
  float RS; 
  float ratio; 
  float data = analogRead(mq135_pin);
  float RL = 10.0;
  float RO = 7.1; // điều chỉnh tuỳ vào lượng khí muốn đo -> cái nầy đo CO2
  float m = -0.417; // nt
  float b = 0.858; // nt

  VRL = data*(5.0/1023.0); //Konversi nilai analog ke digital
  RS = ((5.0*RL)/VRL)-RL;
  ratio = RS/RO; 
  float gas = pow(10, ((log10(ratio)-b)/m));
  float ppm = abs((gas)-0.14);
  return ppm;
}