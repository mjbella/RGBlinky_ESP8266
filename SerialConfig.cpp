#include "Arduino.h"
#include "SerialConfig.h"
#include "Constants.h"
#include <EEPROM.h>

SerialConfigChangedCallbackType BrightnessChangedCallback;
SerialConfigChangedCallbackType LedCountChangedCallback;

uint8_t ConfigLedCount;
uint8_t ConfigBrightness;

//here are the defaults.
#define DEFAULT_LED_COUNT   20
#define DEFAULT_BRIGHTNESS  50
#define EEPROM_SIZE 512

void DumpEeprom();
bool ReadEeprom( uint8_t* brightness, uint8_t* led_count );
bool WriteEeprom( uint8_t brightness, uint8_t led_count );
void ClearEeprom();

void SaveSettings();
void LoadSettings();



void InitSerialConfig(SerialConfigChangedCallbackType brightness_changed,
                      SerialConfigChangedCallbackType led_count_changed)
{
  BrightnessChangedCallback = brightness_changed;
  LedCountChangedCallback = led_count_changed;
  EEPROM.begin(EEPROM_SIZE);
  LoadSettings();
}

void ProcessSerial()
{
  //here we check for commands.
  if ( Serial.available() == 0 )
    return;

  String rx = Serial.readStringUntil('\n');

  if ( rx == "read")
  {
    uint8_t brightness, led_count;
    bool success_read = ReadEeprom( &brightness, &led_count );
    if ( success_read )
    {
      Serial.print("read brightness: ");
      Serial.println(brightness);
      Serial.print("led count: ");
      Serial.println(led_count);
    }
    else
    {
      Serial.println("failed to find valid data, try dumping contents");
    }
    return;
  }

  if ( rx == "clear" )
  {
    Serial.println("clearing eeprom..");
    ClearEeprom();

    return;
  }

  if ( rx == "dump" )
  {
    DumpEeprom();
    return;
  }

  if ( rx == "test" )
  {
    SaveSettings();
    return;
  }

  String p1 = "ledcount:";
  String p2 = ";brightness:";
  String p3 = ";";

  int p1i = rx.indexOf(p1);
  int p2i = rx.indexOf(p2,p1i+p1.length());
  int p3i = rx.indexOf(p3, p2i+p2.length());
  if( p1i >= 0 && p2i >= 0 && p3i >= 0 )
  {
    int ledcount = rx.substring(p1i+p1.length(), p2i).toInt();
    int brightness = rx.substring(p2i+p2.length(), p3i).toInt();

    if( ledcount > MAX_LED_COUNT )
    {
      Serial.print("led count out of range, max: ");
      Serial.println(MAX_LED_COUNT);
      return;
    }
    Serial.print("rx brightness: " );
    Serial.println(brightness);
    Serial.print("rx ledcount: " );
    Serial.println(ledcount);

    bool write_success = WriteEeprom( brightness, ledcount );
    if( write_success )
    {
      Serial.println("values written successfully!");
      ConfigBrightness = brightness;
      ConfigLedCount = ledcount;

      BrightnessChangedCallback(ConfigBrightness);
      LedCountChangedCallback(ConfigLedCount);
    }
    
    return;
  }

  Serial.print("command rx: ");
  Serial.println(rx);
  //commands:
  //"ledcount:55;brightness:255;\n"
  //read
  //clear
}

uint8_t GetConfigBrightness()
{
  return ConfigBrightness;
}

uint8_t GetConfigLedCount()
{
  return ConfigLedCount;
}

void SaveSettings()
{
  WriteEeprom( ConfigBrightness, ConfigLedCount );
}

void DumpEeprom()
{
  Serial.println("dumping EEPROM...");
  for (int i = 0; i < 10; ++i )
  {
    Serial.println( EEPROM.read(i) );
  }
  Serial.println("done");
}

bool ReadEeprom( uint8_t* brightness, uint8_t* led_count )
{
  uint8_t expect_buffer[] = {'M', 'A', 'R', 'K', ','};
  int index = 0;
  for ( int i = 0; i < sizeof(expect_buffer); ++i )
  {
    if ( EEPROM.read(index++) != expect_buffer[i] )
    {
      Serial.print("ReadEeprom: invalid character found at: ");
      Serial.println(index - 1);
      return false;
    }
  }

  *brightness = EEPROM.read(index++);
  if ( EEPROM.read(index++) != ',' )
  {
    Serial.print("ReadEeprom: invalid character found at: ");
    Serial.println(index - 1);
    return false;
  }
  *led_count = EEPROM.read(index++);
  if ( EEPROM.read(index++) != ',' )
  {
    Serial.print("ReadEeprom: invalid character found at: ");
    Serial.println(index - 1);
    return false;
  }
  uint8_t checksum = EEPROM.read(index++);
  uint8_t expect_checksum = *brightness ^ *led_count ^ 'M';
  if ( checksum != expect_checksum )
  {
    Serial.println("ReadEeprom: checksum error");
    return false;
  }

  return true;
}

bool WriteEeprom( uint8_t brightness, uint8_t led_count )
{
  uint8_t checksum = brightness ^ led_count ^ 'M';
  uint8_t write_buffer[] = { 'M', 'A', 'R', 'K', ',', brightness, ',', led_count, ',', checksum };
  int count = sizeof(write_buffer);
  for ( int i = 0; i < count; ++i )
    EEPROM.write(i, write_buffer[i] );

  EEPROM.commit();
  uint8_t test_brightness, test_led_count;

  bool read_back_success = ReadEeprom( &test_brightness, &test_led_count );

  if ( !read_back_success )
  {
    Serial.println("WriteEeprom: readback failed.");
    return false;
  }

  if ( (test_brightness != brightness) || (test_led_count != led_count))
  {
    Serial.println("WriteEeprom: values not stored properly.");
    Serial.println( brightness );
    Serial.println( test_brightness );
    Serial.println( led_count );
    Serial.println( test_led_count );
    return false;
  }
  //write success!
  return true;
}

void ClearEeprom()
{
  //storage is 10 bytes.
  for (int i = 0; i < 10; i++)
    EEPROM.write(i, 0);

  EEPROM.commit();
}

void LoadSettings()
{
  //run once at startup.
  uint8_t brightness, led_count;
  bool success = ReadEeprom( &brightness, &led_count );
  if ( success )
  {
    ConfigBrightness = brightness;
    ConfigLedCount = led_count;
    return;
  }
  Serial.println("failed to load saved settings, using defaults");
  ConfigBrightness = DEFAULT_BRIGHTNESS;
  ConfigLedCount = DEFAULT_LED_COUNT;
}
