//int photo1 = A8;
//int laser1 = 23;

int photo1 = A6;
int laser1 = 21;

int photo2 = A6;
int laser2 = 21;

int photo3 = A6;
int laser3 = 18;



int led = 13;



int high, low, sum;
float average;

void setup() {
  pinMode(laser1, OUTPUT);
  pinMode(photo1, INPUT);
  pinMode(laser2, OUTPUT);
  pinMode(photo2, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9800);

}

void loop() {
  sum = 0;
  for(int i = 0; i < 5; i++) {
    digitalWrite(laser1, HIGH);
    digitalWrite(laser2, HIGH);
    delay(50);
    high = analogRead(photo1);
    digitalWrite(laser1, LOW);
    digitalWrite(laser2, LOW);
    delay(50);
    low = analogRead(photo1);
//    Serial.print(high);
//    Serial.print(" ");
//    Serial.println(low);
    sum = sum + (high - low);
  }
  average = sum / 5.0;
  Serial.println(average);
  if (average >= 100) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
  
}
