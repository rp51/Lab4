#include <Wire.h>
#include <QMC5883LCompass.h>
#include <Adafruit_HTU21DF.h>

Adafruit_HTU21DF temphumSensor = Adafruit_HTU21DF();
QMC5883LCompass compassSensor;

struct temphumData {
  float temperature;
  float humidity;
};

struct compassData {
  float x;
  float y;
  float z;
};

void setup() {
  Serial.begin(9600);
  Wire.begin(25, 26); // Initialize I2C in those specific pins
  // Initialize sensors (by library example)
  if (!temphumSensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while(1); // So it does not continue working
  }
  // Initialize sensors (by library example)
  compassSensor.init();

  // Semaphore for I2C bus access.
  I2C = xSemaphoreCreateBinary();

  // 2 Mailboxes for the sensors.
  QueueHandle_t temphumQueue = xQueueCreate(10, sizeof(temphumData));
  QueueHandle_t compassQueue = xQueueCreate(10, sizeof(compassData));

  // Tasks

}

void loop() {
  // put your main code here, to run repeatedly:

}
