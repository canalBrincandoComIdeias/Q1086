/*
  Sketch: Controle de giro - MPU6050
  Author: Brincando com Ideias
*/

#include "mpu6050.h"
#include <PushButton.h>

PushButton bot(7);

void setup() {
  Serial.begin(9600);

  mpu_begin();

  Serial.println("Calibrando, deixa parado!");
  delay(1000);
  mpu_calibrate(200);
  Serial.println("===== Calibrado! =====\n");

  mpu_reset();
}

void loop() {
  mpu_loop();
  bot.button_loop();

  //Serial.print("roll(x):");Serial.println(getAngleX());
  //Serial.print("pitch(y):");Serial.println(getAngleY());
  //Serial.print("yaw(z):");
  Serial.println(getAngleZ());
  
  if(bot.pressed()) {
    mpu_calibrate(400);
    mpu_reset();
  }

}
