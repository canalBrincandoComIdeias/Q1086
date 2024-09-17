/*
  Sketch: Controle de giro - MPU6050
  Author: Brincando com Ideias
*/

#include "mpu6050.h"
#include "motorDC.h"

//tempo de cada estado
int tempoEstado = 1700;

int angulo;     
int anguloAlvo; 

int estado = 0;
int estadoAnt = 99;
unsigned long delayEstado;

void setup() {
  Serial.begin(9600);

  // Setup dos motores
  pinMode(mtDirF, OUTPUT);
  pinMode(mtDirT, OUTPUT);
  pinMode(mtEsqF, OUTPUT);
  pinMode(mtEsqT, OUTPUT);

  parar();  // Garantir os motores parados

  mpu_begin();

  Serial.println("Calibrando, deixa parado!");
  delay(1000);
  mpu_calibrate(400);
  Serial.println("===== Calibrado! =====\n");

  delay(20);
  delayEstado = millis();
}

void loop() {
  mpu_loop();
  loopDC();
  angulo = getAngleZ();

  //Serial.println(angulo);

  if ((millis() - delayEstado) > tempoEstado) {
     delayEstado = millis();
     estado++;

     if (estado > 12) {
        estado = 0;
     }
  }

  //estado = 0: Parado, reseta o giroscopio
  if (estado == 0) {
    if (estadoAnt != estado) {
      mpu_reset();
      angulo = getAngleZ();
    }
  }

  if (estado == 2 || estado == 5 || estado == 8 || estado == 11) {
    if (estadoAnt != estado) {
       parar();
       tempoEstado = 2000;
    }
  }

  if (estado == 1 || estado == 4 || estado == 7 || estado == 10 ) {
    if (estadoAnt != estado) {
      frente();
      tempoEstado = 1000;
    }
  }

  if ((estado == 3 || estado == 12) && estadoAnt != estado) {
    girarDireita();
    tempoEstado = 3000;
  }
  if (estado == 3 && angulo <= -90) {
    parar();
  }
  if (estado == 12 && angulo <= 0) {
    parar();
  }

  if ((estado == 6 || estado == 9) && estadoAnt != estado) {
    girarEsquerda();
    tempoEstado = 3000;
  }
  if (estado == 6 && angulo >= 0) {
    parar();
  }
  if (estado == 9 && angulo >= 90) {
    parar();
  }


  estadoAnt = estado;
}


