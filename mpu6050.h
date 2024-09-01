#include <Wire.h>

const int MPU = 0x68;  // MPU6050 I2C address

float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;

float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;

float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;

/* Função para configurar o MPU */
void mpu_begin() {
  Wire.begin();                 // Inicializa comunicação I2C
  Wire.beginTransmission(MPU);  // Começa a transmissão para o MPU=0x68
  Wire.write(0x6B);             // Reinicia o MPU
  Wire.write(0x00);             // Reinicia o MPU
  Wire.endTransmission(true);   // Finaliza a transmissão

  // Configuração da faixa do Acelerômetro
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);  // Registrador de conf. do acelerômetro
  Wire.write(0);  // Faixa: 2g
  Wire.endTransmission(true);

  // Configuração da faixa do Giroscópio
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);  // Registrador de conf. do giroscópio
  Wire.write(0);  // faixa: 250°/s
  Wire.endTransmission(true);

  delay(20);
}

/* Realiza a leitura do acelerômetro e salva os valores recebidos */
void readAcel() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  //Para faixa de +-2g divisor = 16384
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0f;  // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0f;  // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0f;  // Z-axis value
}

/* Realiza a leitura do giroscópio e salva os valores recebidos */
void readGiro() {
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  // Para faixa de +-250°/s divisor = 131.0 
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0f;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0f;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0f;
}

/* Função para calibrar o MPU, realixa várias leituras quando totalmente parado e calcula o valor para ajustar o calculo do angulo */
void mpu_calibrate(int leituras) {

  // Realiza leituras do acelerômetro  
  for (int nR = 0; nR < leituras; nR++) {
    readAcel();
    
    AccErrorX += (atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI);
    AccErrorY += (atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI);
  }
  
  AccErrorX = AccErrorX / (float)leituras;
  AccErrorY = AccErrorY / (float)leituras;
  

  // Realiza leituras do giroscópio
  for (int nR = 0; nR < leituras; nR++) {
    readGiro();

    GyroErrorX += GyroX;
    GyroErrorY += GyroY;
    GyroErrorZ += GyroZ;
  }

  GyroErrorX = GyroErrorX / (float)leituras;
  GyroErrorY = GyroErrorY / (float)leituras;
  GyroErrorZ = GyroErrorZ / (float)leituras;
}

/* Função para realizar as leiturar e calcular os angulos dos três eixos, deve ser chamada contínuamente para atualizar os angulos, evite delay entre as leituras */
void mpu_loop() {
  //Leitura do Acelerômetro
  readAcel();

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY;

  // === Read gyroscope (on the MPU6050) data === //
  previousTime = currentTime;
  currentTime = micros();
  elapsedTime = (currentTime - previousTime) / 1000000.0f;  // Divide by 1000000 to get seconds

  readGiro();
  // Correct the outputs with the calculated error values
  GyroX -= GyroErrorX;
  GyroY -= GyroErrorY;
  GyroZ -= GyroErrorZ;

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX += GyroX * elapsedTime;  // deg/s * s = deg
  gyroAngleY += GyroY * elapsedTime;
  gyroAngleZ += GyroZ * elapsedTime;

  //combine accelerometer- and gyro-estimated angle values. 0.96 and 0.04 values are determined through trial and error by other people
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  yaw = gyroAngleZ;
}

/* Função para reiniciar o giroscopio */
void mpu_reset() {
  gyroAngleX = 0;
  gyroAngleY = 0;
  gyroAngleZ = 0;
  
  // reinicia o tempo base do giroscópio
  currentTime = micros();
  mpu_loop();
}

/* Retorna o angulo de giro em Z */
float getAngleX() {
  return roll;
}

float getAngleY() {
  return pitch;
}

float getAngleZ() {
  return yaw;
}

/* Retorna o valor que falta para atingir o angulo target */
int getDeltaZ(int target) {
  if (target < 0) return (round(getAngleZ() - target));
  return (round(target - getAngleZ()));
}