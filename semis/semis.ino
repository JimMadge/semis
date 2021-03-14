#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <BME280I2C.h>
#include <SPI.h>
#include <Wire.h>

/* Initialise BME280 using default settings (forced mode, 1000 ms standby time,
 * pressure ×1, temperature ×1, humidity ×1, filter off) */
BME280I2C bme;

// Initialise ST7789
// chip select pin
#define TFT_CS 10
// data/command pin
#define TFT_DC 9
// reset pin
#define TFT_RST 8
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// TFT rotation [0, 1, 2, 3]
#define TFT_ROTATION 0

// Serial BAUD RATE
#define BAUD_RATE 9600


void setup() {
    Serial.begin(BAUD_RATE);

    while(!Serial) {}

    Wire.begin();

    while(!bme.begin()) {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
    }

    // Initialise st7899 240x240 pixels
    tft.init(240, 240, SPI_MODE0);
    // Set rotation
    tft.setRotation(TFT_ROTATION);
    // Fill screen with black
    tft.fillScreen(ST77XX_BLACK);

    // Write measurement headers
    // Set text colour to white on black
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(3);
    tft.setCursor(17, 0);
    tft.print("TEMPERATURE:");
    tft.setCursor(44, 89);
    tft.print("HUMIDITY:");
    tft.setCursor(44, 178);
    tft.print("PRESSURE:");

    // Set text settings for writing values (in loop)
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setTextSize(4);
    tft.setTextWrap(false);
}

// Declare measurment variables
float temperature, humidity, pressure;

// Declare temperature and pressure units (default Celcius and Pa)
BME280::TempUnit temperature_unit(BME280::TempUnit_Celsius);
BME280::PresUnit pressure_unit(BME280::PresUnit_hPa);


void loop() {
    bme.read(pressure, temperature, humidity, temperature_unit, pressure_unit);

    print_to_serial(temperature, pressure, humidity);

    tft.setCursor(48, 34);
    tft.print(String(temperature, 1));
    tft.println("°C");

    tft.setCursor(39, 123);
    tft.print(String(humidity, 1));
    tft.println("%");

    tft.setCursor(30, 212);
    tft.print(round(pressure));
    tft.println("hPa");

    delay(5000);
}


void print_to_serial(float temperature, float pressure, float humidity) {
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println("°C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println("hPa");
    Serial.println();
}
