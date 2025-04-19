#define trigPin1 2
#define echoPin1 15
#define trigPin2 12
#define echoPin2 13

#define irSensor1 34
#define irSensor2 35

#define buzzerPin 25
#define redLED 32
#define greenLED 33

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // in cm
  return distance;
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(irSensor1, INPUT);
  pinMode(irSensor2, INPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

void loop() {
  long distance1 = getDistance(trigPin1, echoPin1);
  long distance2 = getDistance(trigPin2, echoPin2);

  int ir1 = digitalRead(irSensor1);
  int ir2 = digitalRead(irSensor2);

  bool alert = false;

  Serial.print("Ultrasonic 1 (cm): ");
  Serial.print(distance1);
  Serial.print(" | Ultrasonic 2 (cm): ");
  Serial.print(distance2);
  Serial.print(" | IR1: ");
  Serial.print(ir1);
  Serial.print(" | IR2: ");
  Serial.println(ir2);

  // Crack detection
  if (ir1 == 1) {
    Serial.println("⚠️ ALERT: Crack detected by IR Sensor 1");
    alert = true;
  }

  if (ir2 == 1) {
    Serial.println("⚠️ ALERT: Crack detected by IR Sensor 2");
    alert = true;
  }

  // Misalignment detection (distance > 6 OR ≤ 2)
  if (distance1 > 6 || distance1 <= 2) {
    Serial.println("⚠️ ALERT: Misalignment detected by Ultrasonic Sensor 1");
    alert = true;
  }

  if (distance2 > 6 || distance2 <= 2) {
    Serial.println("⚠️ ALERT: Misalignment detected by Ultrasonic Sensor 2");
    alert = true;
  }

  // Alert handling
  if (alert) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzerPin, LOW);
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzerPin, HIGH);
  }

  delay(1500); // Adjust as needed
}
