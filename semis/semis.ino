#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <BME280I2C.h>
#include <Fonts/FreeSans18pt7b.h>
#include <SPI.h>
#include <Wire.h>

/* Initialise BME280 using default settings (forced mode, 1000 ms standby time,
 * pressure ×1, temperature ×1, humidity ×1, filter off) */
BME280I2C bme;

// Initialise ST7789
// chip select pin
const int TFT_CS = 10;
// data/command pin
const int TFT_DC = 9;
// reset pin (-1 means unused)
const int TFT_RST = -1;
// TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Backlight pin (should be PWM)
const int TFT_BL = 3;
// On brightness
const int TFT_ON = 255/2;

// TFT rotation [0, 1, 2, 3]
const int TFT_ROTATION = 0;

// Serial BAUD RATE
const int BAUD_RATE = 9600;

// Declare measurment variables
float temperature, humidity, pressure;
// Declare temperature and pressure units (default Celcius and Pa)
BME280::TempUnit temperature_unit(BME280::TempUnit_Celsius);
BME280::PresUnit pressure_unit(BME280::PresUnit_hPa);


void setup() {
    Serial.begin(BAUD_RATE);

    while(!Serial) {}

    Wire.begin();

    while(!bme.begin()) {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
    }

    // Set backlight pin
    pinMode(TFT_BL, OUTPUT);
    analogWrite(TFT_BL, TFT_ON);

    // Initialise st7899 240x240 pixels
    tft.init(240, 240, SPI_MODE0);
    // Set rotation
    tft.setRotation(TFT_ROTATION);
    // Fill screen with black
    tft.fillScreen(ST77XX_BLACK);

    // Write measurement headers
    tft.setFont(&FreeSans18pt7b);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, row_pixel(0));
    tft.print("T:");
    tft.setCursor(0, row_pixel(1));
    tft.print("H:");
    tft.setCursor(0, row_pixel(2));
    tft.print("P:");

    // Set text settings for writing values (in loop)
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setTextWrap(false);

    // Get initial reading
    bme.read(pressure, temperature, humidity, temperature_unit, pressure_unit);
}


// Variables for writing text to st7899
int16_t  x1, y1;
uint16_t w, h;
String buffer;

// Indent of readings in pixels (i.e. leave space for the label)
const unsigned int INDENT = 50;

// Time since last reading and delay between readings in milliseconds
unsigned long last_reading = 0;
const unsigned long reading_delay = 5000;
unsigned long time;


void loop() {
    time = millis();
    if (time - last_reading > reading_delay) {
        last_reading = time;

        bme.read(pressure, temperature, humidity, temperature_unit, pressure_unit);

        print_to_serial(time, temperature, pressure, humidity);

        buffer = String(temperature, 1) + String("°C");
        print_to_tft(buffer, INDENT, row_pixel(0));

        buffer = String(humidity, 1) + String("%");
        print_to_tft(buffer, INDENT, row_pixel(1));

        buffer = String(round(pressure)) + String("hPa");
        print_to_tft(buffer, INDENT, row_pixel(2));
    }
}


int row_pixel(int row_number) {
    return (36+18)*row_number + 36;
}


void print_to_tft(String buffer, int x, int y) {
    tft.setCursor(x, y);
    tft.getTextBounds(buffer, x, y, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, ST77XX_BLACK);
    tft.drawRect(x1, y1, w, h, ST77XX_BLACK);
    tft.println(buffer);
}


void print_to_serial(unsigned long time, float temperature, float pressure,
                     float humidity) {
    Serial.print("Time: ");
    Serial.print(time);
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
