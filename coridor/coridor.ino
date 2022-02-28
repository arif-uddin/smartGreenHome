
int pin = 7;

const int trigPin1 = 4;
const int echoPin1 = 5;

int motionValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pin, INPUT);
  pinMode(8, OUTPUT);
  pinMode(5, INPUT);
  pinMode(6, OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  digitalWrite(8,1);
  digitalWrite(6,1);
  Serial.println("Waiting For Ready");
  delay(60000);
  Serial.println("Ready");
  
 
}

long microsecondsToCentimeters(int mic)
{
  return mic/29/2;
}

void loop() {

  motionValue = digitalRead(pin);

  if (motionValue == HIGH) {
    digitalWrite(8, 0);
    Serial.println("Motion Detected!");
    

  } else {
    digitalWrite(8, 1);
    Serial.println("Motion Ende!");
  }

  long duration1, cm1;

  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn(echoPin1, HIGH);

  cm1 = microsecondsToCentimeters(duration1);

  Serial.print("Distance=");
  Serial.println(cm1);
  delay(200);

  if(cm1>20 || cm1<18)
  {
    digitalWrite(6,0);
  }
  else
  {
    digitalWrite(6,1);
  }
  

  
  


}
