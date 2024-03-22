#include <Wire.h>
#include <QMC5883LCompass.h>
#include <Adafruit_HTU21DF.h>

SemaphoreHandle_t I2C;
QueueHandle_t temphumQueue;
QueueHandle_t compassQueue;

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
  xSemaphoreGive(I2C);   

  // 2 Mailboxes for the sensors.
  temphumQueue = xQueueCreate(1, sizeof(temphumData));
  compassQueue = xQueueCreate(1, sizeof(compassData));

  // Tasks
  xTaskCreate(temphumTask, "Task for TempHum Sensors", 2048, NULL, 1, NULL);
  xTaskCreate(compassTask, "Task for Compass", 2048, NULL, 1, NULL);
  xTaskCreate(receiverTask, "Tasks for Receiving Data", 2048, NULL, 1, NULL);

}

void temphumTask(void *Parameters) {
  temphumData data;

  while(1) {
      xSemaphoreTake(I2C, portMAX_DELAY);
      data.temperature = temphumSensor.readTemperature();
      data.humidity = temphumSensor.readHumidity();
      xSemaphoreGive(I2C);
      vTaskDelay(500);
      xQueueOverwrite(temphumQueue, &data);
    
  }
  
}

void compassTask(void *Parameters) {
  compassData data;

  while(1) {
    xSemaphoreTake(I2C, portMAX_DELAY);
    compassSensor.read();
    data.x = compassSensor.getX();
    data.y = compassSensor.getY();
    data.z = compassSensor.getZ();
    xSemaphoreGive(I2C);      
    vTaskDelay(100);    
    xQueueOverwrite(compassQueue, &data);    
  }
}

void receiverTask(void *Parameters) {
  temphumData data1;
  compassData data2;

  while(1) {
    if (xQueueReceive(temphumQueue, &data1, portMAX_DELAY) == pdTRUE) {
      Serial.print("Temperature:");
      Serial.println(data1.temperature);
      Serial.print("Humidity:");
      Serial.println(data1.humidity);
    }
    if (xQueueReceive(compassQueue, &data2, portMAX_DELAY) == pdTRUE) {
      Serial.println("x: ");
      Serial.print(data2.x);
      Serial.println("y: ");
      Serial.print(data2.y);
      Serial.println("z: ");
      Serial.print(data2.z);
    } 
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
