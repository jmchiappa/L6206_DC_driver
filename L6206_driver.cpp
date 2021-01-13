    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#include "Arduino.h"
#include "L6206_driver.h"
#define PWM_RES		255
#define ZERO_THRESHOLD 	20
#define MIDDLE(minV,maxV)	((minV+maxV)/2)
L6206::L6206(uint8_t enaPin, uint8_t pwmPin, uint8_t dirPin)
{
	this->enaPin  = enaPin;
	this->pwmPin = pwmPin;
	this->dirPin  = dirPin;
	//begin();
	RefreshPeriod = 5;
	_initialized = false;
}

void L6206::begin(uint32_t Period)
{
//	Serial.begin(9600);
	// Serial.print("initializing ");
	// Serial.print(this->enaPin);
	pinMode(this->enaPin,OUTPUT);
	// Serial.print("\t");
	// Serial.print(this->dirPin);
	pinMode(this->dirPin,OUTPUT);
	// Serial.print("\t");
	// Serial.print(this->pwmPin);
	pinMode(this->pwmPin,OUTPUT);
	digitalWrite(this->enaPin,HIGH); // désactive le pont
	digitalWrite(this->dirPin,LOW); // marche avant
	analogWrite(this->pwmPin,0);
	_initialized = true;
	MaxOutputSpeedCoef = 100 ; // Set by default to max speed
	this->dir=FORWARD;
	this->RefreshPeriod = Period;
}

void L6206::SetInputLimit(int32_t minValue, int32_t maxValue)
{
	if(!_initialized) begin(RefreshPeriod);
	this->minValue = minValue;
	this->maxValue = maxValue;
	SetReference(MIDDLE(minValue,maxValue));
//	Serial.print("Set Min Value=");
//	Serial.println(minValue);
//	Serial.print("Set Max Value=");
//	Serial.println(maxValue);
}
/* set the value for a zero output. it calibrates gain and offset
* ZeoValue should be set between min and max value
*/
void L6206::SetReference(int32_t ZeroValue)
{
	if(!_initialized) begin(RefreshPeriod);
	this->ZeroValue = constrain(ZeroValue,minValue,maxValue);
//	Serial.print("Set Zero Value=");
//	Serial.println(ZeroValue);
}

/* Set Limit to output speed expressed as percent of PWM_RES
* input : int comprised between 0 to 100
* output : none, implicit output clamping to PWM_RES * percent / 100
*/
void L6206::SetSpeedLimit(int32_t percent)
{
	if(!_initialized) begin(RefreshPeriod);
	this->MaxOutputSpeedCoef = constrain(percent,0,100);
}

// applied immediately a speed value (forward or backward)
void L6206::ForceSpeed(int speedValue)
{
	if(!_initialized) begin(RefreshPeriod);
	int32_t s = constrain(speedValue,minValue,maxValue);	
	s-=ZeroValue;
	SetSpeedMotor(s);
}

/* Set pwm value according to the remap speedValue between minValue and maxValue
* speedValue < 0 : rear value
* speedValue > 0 : forward value
*
*/
void L6206::SetSpeed(int speedValue)
{
	if(!_initialized) begin(RefreshPeriod);
	// set speed value between min and max
	sumSamples+=speedValue;
	nbSamples++;
	if(millis() > (PreviousRefreshTime+RefreshPeriod))
	{
		PreviousRefreshTime = millis();
		sumSamples/=nbSamples;
		// Serial.print(sumSamples);
		// Serial.print(",");
		int32_t s = constrain(sumSamples,minValue,maxValue);	
		// Serial.print(s);
		// Serial.print(",");
		nbSamples=0;
		sumSamples=0;
		// remap value in pwm space
		s-=ZeroValue;
		// Serial.print("input speed=");
		// Serial.print(sumSamples);
		// Serial.print("\tcentred speed=");
		// Serial.print(s);
		// Serial.print(",");
		SetSpeedMotor(s);
	}
}

void L6206::SetSpeedMotor(int32_t s)
{
    uint32_t moteur;

    if(s>=ZERO_THRESHOLD)
    {
	    moteur = map(s,ZERO_THRESHOLD,maxValue-ZeroValue ,0,PWM_RES); //*sign(y-x);
	    //moteur=(LUT_MOTEUR[moteur]*MaxOutputSpeedCoef) / 100;
			moteur=(moteur*MaxOutputSpeedCoef) / 100;
			// Serial.print(moteur);
			// Serial.print(",");
	    if(this->dir!=FORWARD){
	    	this->dir=FORWARD;
    		digitalWrite(this->dirPin,LOW);
    	}
    	Speed = moteur;
		// Serial.print("\tpos m=");
    }
    else if(s<=-ZERO_THRESHOLD)
    {
	    moteur = map(s,-ZeroValue,-ZERO_THRESHOLD ,PWM_RES,0); //*sign(y-x);
	    // Speed = -(int32_t)(moteur*MaxOutputSpeedCoef)/100;
    	// moteur=PWM_RES-(LUT_MOTEUR[moteur]*MaxOutputSpeedCoef)/100;
			// Serial.print(moteur);
			// Serial.print(",");
    	moteur=(moteur*MaxOutputSpeedCoef)/100;
    	moteur = PWM_RES-moteur;
//    	Speed=(int32_t)moteur-PWM_RES;
    	Speed=moteur;
	    if(this->dir!=BACKWARD){
	    	this->dir = BACKWARD;
    		digitalWrite(this->dirPin,HIGH);	
    	}
		// Serial.print("\trev m=");
    }
    else // moteur à l'arrêt)
    {
	    if(this->dir!=FORWARD){
	    	this->dir=FORWARD;
	    	digitalWrite(this->dirPin,LOW);
	    }
    	moteur=0;
    	Speed=0;
		// Serial.print("\tstop m=");
    }
    // moteur=LUT_MOTEUR[moteur];
	// Serial.print(";");
	// Serial.println(moteur);
	// Serial.println(moteur);
		//Serial.println(moteur);
    analogWrite(this->pwmPin,moteur);
}


int32_t L6206::getZeroOrigin(void)
{
	return ZeroValue;
}

int32_t L6206::getSpeed(void)
{
	return Speed;
}
