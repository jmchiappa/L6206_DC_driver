#include "L6206_driver.h"

#define AXE_X   A3
#define AXE_Y   A5
#define ENA		2
#define DIRA 	4
#define PWMA	5

#define ENB		50
#define DIRB 	47
#define PWMB	46

L6206 moteurGauche(ENB,PWMB,DIRB);
L6206 moteurDroite(ENA,PWMA,DIRA);


void setup() {
    Serial.begin(115200);
	moteurDroite.SetInputLimit(0,1023);
	moteurDroite.SetReference(analogRead(AXE_Y));
	moteurGauche.SetInputLimit(0,1023);
	moteurGauche.SetReference(analogRead(AXE_X));
}

void loop() {
//    int32_t vd;
//    Serial.print("y=");
//    Serial.print(y);
    moteurGauche.SetSpeed(analogRead(AXE_X));
    moteurDroite.SetSpeed(analogRead(AXE_Y));
//    Serial.print(" vd=");
//    Serial.print(vd);
//    Serial.print(" dy=");
//    Serial.println(y);
}
