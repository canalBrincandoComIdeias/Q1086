#define mtDirF 11
#define mtDirT 10
#define mtEsqF 9
#define mtEsqT 6

#define VELOCIDADE 75  //Deve ser multiplo de 5
#define IMPULSO 200    //Deve ser multiplo de 5

int mtVelocidade = 0;   //de 0(parado) até 255(mais rápido)
int mtSentido    = 0;   //0=parado 1=frente 2=direita 3=esquerda 4=atrás 11=impulso frente 12=impulso direita 13=impulso esquerda 14=impulso atras

int mtVelDest  = 0;
int mtSentDest = 0;

int mtRampaVel = 1;    //10 milisegundos para variar uma unidade da velocidade na rampa de alteração
unsigned long mtDelayVel = 0;

void comandoDC(int Sent, int Vel) {
  mtVelDest  = Vel;
  mtSentDest = Sent;
}

void girarDireita() {
  comandoDC(2, VELOCIDADE);
}

void girarEsquerda() {
  comandoDC(3, VELOCIDADE);
}

/* Desliga os motores e faz um "delay" esperando a parada completa enquanto faz a leitura do angulo*/
void parar() {
  comandoDC(0, 0);
}

void frente() {
  comandoDC(1, VELOCIDADE);
}

void loopDC() {
  if (mtVelocidade != mtVelDest) {

     if ((millis() - mtDelayVel) > mtRampaVel) {
        mtDelayVel = millis();
        if (mtVelocidade > mtVelDest) {
          mtVelocidade -= 5;
          if (mtVelocidade < mtVelDest) {
            mtVelocidade = mtVelDest;
          }
        }
        if (mtVelocidade < mtVelDest) {
          mtVelocidade += 5;
          if (mtVelocidade > mtVelDest) {
            mtVelocidade = mtVelDest;
          }
        }
     }
  }

  //Esta parado e vai mudar
  if (mtSentido != mtSentDest) {

    //parar
    if (mtSentDest == 0) {
      mtSentido = 0;
      mtVelocidade = 0;
     
      digitalWrite(mtEsqF, LOW);
      digitalWrite(mtEsqT, LOW);
      digitalWrite(mtDirF, LOW);
      digitalWrite(mtDirT, LOW);
    }

    //vai para frente
    if (mtSentDest == 1) {
      mtSentDest = 11;
      mtVelocidade = IMPULSO;
    }
    //impulso para frente
    if (mtSentDest == 11) {  
      analogWrite(mtEsqF, mtVelocidade);
      analogWrite(mtEsqT, 0);
      analogWrite(mtDirF, mtVelocidade);
      analogWrite(mtDirT, 0);

      if (mtVelocidade == mtVelDest) {
          mtSentDest = 1;
          mtSentido = 1;
      }
    }

    //vai para tras
    if (mtSentDest == 4) {
      mtSentDest = 14;
      mtVelocidade = IMPULSO;
    }
    //impulso para tras
    if (mtSentDest == 14) {
      analogWrite(mtEsqF, 0);
      analogWrite(mtEsqT, mtVelocidade);
      analogWrite(mtDirF, 0);
      analogWrite(mtDirT, mtVelocidade);

      if (mtVelocidade == mtVelDest) {
          mtSentDest = 4;
          mtSentido = 4;
      }
    }

    //vai para direita
    if (mtSentDest == 2) {
      mtSentDest = 12;
      mtVelocidade = IMPULSO;
    }
    //impulso para direita
    if (mtSentDest == 12) {  
      analogWrite(mtEsqF, mtVelocidade);
      analogWrite(mtEsqT, 0);
      analogWrite(mtDirF, 0);
      analogWrite(mtDirT, mtVelocidade);

      if (mtVelocidade == mtVelDest) {
          mtSentDest = 2;
          mtSentido = 2;
      }
    }

    //vai para esquerda
    if (mtSentDest == 3) {
      mtSentDest = 13;
      mtVelocidade = IMPULSO;
    }
    //impulso para esquerda
    if (mtSentDest == 13) {  
      analogWrite(mtEsqF, 0);
      analogWrite(mtEsqT, mtVelocidade);
      analogWrite(mtDirF, mtVelocidade);
      analogWrite(mtDirT, 0);

      if (mtVelocidade == mtVelDest) {
          mtSentDest = 3;
          mtSentido = 3;
      }
    }
  }
}