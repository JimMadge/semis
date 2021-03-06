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
const int TFT_ON = 255/3;
// Off brightness
const int TFT_OFF = 0;

// TFT rotation [0, 1, 2, 3]
const int TFT_ROTATION = 0;

// Serial BAUD RATE
const int BAUD_RATE = 9600;

// Declare measurment variables
float temperature, humidity, pressure;
float min_temperature, max_temperature;

// Declare temperature and pressure units (default Celcius and Pa)
BME280::TempUnit temperature_unit(BME280::TempUnit_Celsius);
BME280::PresUnit pressure_unit(BME280::PresUnit_hPa);

// Declare display switch pin
const int DISPLAY_PIN = 7;
// Declare min/max reset switch pin
const int RESET_PIN = 8;


void setup() {
    Serial.begin(BAUD_RATE);

    while(!Serial) {}

    Wire.begin();

    while(!bme.begin()) {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
    }

    // Set backlight pin and brightness
    pinMode(TFT_BL, OUTPUT);
    analogWrite(TFT_BL, TFT_ON);

    // Set display switch
    pinMode(DISPLAY_PIN, INPUT_PULLUP);
    // Set min/max reset switch
    pinMode(RESET_PIN, INPUT_PULLUP);

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
    tft.setCursor(0, row_pixel(4));
    tft.print("MIN:");
    tft.setCursor(0, row_pixel(5));
    tft.print("MAX:");

    // Set text settings for writing values (in loop)
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setTextWrap(false);

    // Get initial reading
    bme.read(pressure, temperature, humidity, temperature_unit, pressure_unit);
    min_temperature = temperature;
    max_temperature = temperature;

    // Write initial values to TFT
    update_tft(temperature, pressure, humidity, min_temperature,
               max_temperature);
}


// Indent of readings in pixels (i.e. leave space for the label)
const unsigned int INDENT = 50;
const unsigned int INDENT2 = 100;

// Time since last reading and delay between readings in milliseconds
unsigned long last_reading = 0;
const unsigned long reading_delay = 5000;

// Time since display was switched on and how long to leave on in milliseconds
unsigned long display_started = 0;
const unsigned long display_on_time = 15000;

// Time since start
unsigned long time;


void loop() {
    // Get time since start
    time = millis();

    // Take a reading if `reading_delay` milliseconds have elapsed
    if (time - last_reading > reading_delay) {
        last_reading = time;

        bme.read(pressure, temperature, humidity, temperature_unit, pressure_unit);

        if (temperature < min_temperature) {
            min_temperature = temperature;
        }

        if (temperature > max_temperature) {
            max_temperature = temperature;
        }

        print_to_serial(time, temperature, pressure, humidity);
        update_tft(temperature, pressure, humidity, min_temperature,
                   max_temperature);
    }

    // Turn on the display if the display switch is pressed
    if (digitalRead(DISPLAY_PIN) ==  LOW) {
        display_started = time;
        tft.enableDisplay(true);
        analogWrite(TFT_BL, TFT_ON);
    }

    // Turn off the display if it has been on for `display_on_time` milliseconds
    if (time - display_started > display_on_time) {
        analogWrite(TFT_BL, TFT_OFF);
        tft.enableDisplay(false);
    }

    // Reset min/max temperature if the reset switch is pressed
    if (digitalRead(RESET_PIN) == LOW) {
        min_temperature = temperature;
        max_temperature = temperature;
        update_tft(temperature, pressure, humidity, min_temperature,
                   max_temperature);
    }
}


// Font height in pixels (18pt → 24px)
const int FONT_HEIGHT_PX = 25;
// Extra spacing between rows in pixels
const int ROW_SPACING_PX = 12;


int row_pixel(int row_number) {
    return (FONT_HEIGHT_PX+ROW_SPACING_PX)*row_number + FONT_HEIGHT_PX;
}


void update_tft(float temperature, float pressure, float humidity,
                float min_temperature, float max_temperature) {
    String buffer;

    buffer = String(temperature, 1) + String("°C");
    print_to_tft(buffer, INDENT, row_pixel(0));

    buffer = String(humidity, 1) + String("%");
    print_to_tft(buffer, INDENT, row_pixel(1));

    buffer = String(round(pressure)) + String("hPa");
    print_to_tft(buffer, INDENT, row_pixel(2));

    buffer = String(min_temperature, 1) + String("°C");
    print_to_tft(buffer, INDENT2, row_pixel(4));

    buffer = String(max_temperature, 1) + String("°C");
    print_to_tft(buffer, INDENT2, row_pixel(5));
}


void print_to_tft(String buffer, int x, int y) {
    tft.fillRect(x, y, 240-INDENT, -FONT_HEIGHT_PX-1, ST77XX_BLACK);
    tft.setCursor(x, y);
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
