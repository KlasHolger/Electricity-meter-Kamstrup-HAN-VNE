#pragma once

//----------------------------------------------------------------------------
// Filnamn: elmatare_mbus.h
// Version: elmatare_mbus 1 (converted to external component)
// Ursprung: Klas Jansson 2022-03-05
// Konverterad för ESPHome external component
// Skalar och publicerar sensordata i C++ så att YAML behöver inga extra filters
//----------------------------------------------------------------------------

#include "esphome.h"

class MbusReader : public Component, public uart::UARTDevice {
 public:
  MbusReader(uart::UARTComponent *parent) : uart::UARTDevice(parent) {}
  uint8_t temp_byte = 0;
  uint8_t *temp_byte_pointer = &temp_byte;
  uint8_t uart_buffer_[512]{0};
  uint16_t uart_counter = 0;
  char uart_message[550];
  char temp_string[10];
  char obis_code[32];
  char temp_obis[10];
  uint32_t obis_value = 0;

  // Sensors exposed to ESPHome
  Sensor *wattage_sensor = new Sensor(); // Aktuell effekt (W) -> publiceras i kW
  Sensor *amperageL1_sensor = new Sensor();
  Sensor *amperageL2_sensor = new Sensor();
  Sensor *amperageL3_sensor = new Sensor();
  Sensor *voltageL1_sensor = new Sensor();
  Sensor *voltageL2_sensor = new Sensor();
  Sensor *voltageL3_sensor = new Sensor();
  Sensor *energy_sensor = new Sensor(); // publiceras i kWh
  Sensor *reactive_power_sensor = new Sensor();
  Sensor *reactive_energy_sensor = new Sensor();

  void setup() override {}

  void loop() override { read_message(); }

  bool read_message() {
    while (available() >= 1) {
      read_byte(this->temp_byte_pointer);
      if (temp_byte == 126) {
        if (uart_counter > 2) {
          uart_buffer_[uart_counter] = temp_byte;
          uart_counter++;
          uart_message[0] = '\0';
          strcpy(uart_message, "");
          for (uint16_t i = 1; i < uart_counter && i < 256; i++) {
            if (uart_buffer_[i - 1] == 9 && uart_buffer_[i] == 6) {
              obis_code[0] = '\0';
              strcpy(obis_code, "");
              for (uint16_t y = 1; y < 6; y++) {
                sprintf(temp_obis, "%d.", uart_buffer_[i + y]);
                strcat(obis_code, temp_obis);
              }
              sprintf(temp_obis, "%d", uart_buffer_[i + 6]);
              strcat(obis_code, temp_obis);
              ESP_LOGV("uart", "OBIS code found: %s message length: %d", obis_code, uart_buffer_[i + 7]);
              obis_value = 0;
              if (uart_buffer_[i + 7] == 6) {
                for (uint8_t y = 0; y < 4; y++) {
                  obis_value += (uint32_t)uart_buffer_[i + 8 + y] << ((3 - y) * 8);
                }
              } else if (uart_buffer_[i + 7] == 18) {
                for (uint8_t y = 0; y < 2; y++) {
                  obis_value += (uint32_t)uart_buffer_[i + 8 + y] << ((1 - y) * 8);
                }
              }

              // Konvertera och publicera med rätt skalning (så YAML slipper filters)
              // Wattage: presenteras som kW (observation i W * 0.001)
              if (strcmp(obis_code, "1.1.1.7.0.255") == 0) {
                float val = (float)obis_value * 0.001f;
                ESP_LOGD("elmatare_mbus", "Wattage: %f kW", val);
                wattage_sensor->publish_state(val);
              }
              // Fasström L1
              else if (strcmp(obis_code, "1.1.31.7.0.255") == 0) {
                float val = (float)obis_value * 0.01f;
                ESP_LOGD("elmatare_mbus", "AmperageL1: %f A", val);
                amperageL1_sensor->publish_state(val);
              }
              // Fasström L2
              else if (strcmp(obis_code, "1.1.51.7.0.255") == 0) {
                float val = (float)obis_value * 0.01f;
                ESP_LOGD("elmatare_mbus", "AmperageL2: %f A", val);
                amperageL2_sensor->publish_state(val);
              }
              // Fasström L3
              else if (strcmp(obis_code, "1.1.71.7.0.255") == 0) {
                float val = (float)obis_value * 0.01f;
                ESP_LOGD("elmatare_mbus", "AmperageL3: %f A", val);
                amperageL3_sensor->publish_state(val);
              }
              // Spänning L1
              else if (strcmp(obis_code, "1.1.32.7.0.255") == 0) {
                float val = (float)obis_value;
                ESP_LOGD("elmatare_mbus", "VoltageL1: %f V", val);
                voltageL1_sensor->publish_state(val);
              }
              // Spänning L2
              else if (strcmp(obis_code, "1.1.52.7.0.255") == 0) {
                float val = (float)obis_value;
                ESP_LOGD("elmatare_mbus", "VoltageL2: %f V", val);
                voltageL2_sensor->publish_state(val);
              }
              // Spänning L3
              else if (strcmp(obis_code, "1.1.72.7.0.255") == 0) {
                float val = (float)obis_value;
                ESP_LOGD("elmatare_mbus", "VoltageL3: %f V", val);
                voltageL3_sensor->publish_state(val);
              }
              // Aktiv energi (publiceras i kWh)
              else if (strcmp(obis_code, "1.1.1.8.0.255") == 0) {
                float val = (float)obis_value * 0.001f;
                ESP_LOGD("elmatare_mbus", "Energy: %f kWh", val);
                energy_sensor->publish_state(val);
              }
              // Reaktiv effekt
              else if (strcmp(obis_code, "1.1.4.7.0.255") == 0) {
                float val = (float)obis_value;
                ESP_LOGD("elmatare_mbus", "Reactive Power: %f", val);
                reactive_power_sensor->publish_state(val);
              }
              // Reaktiv energi (hourly)
              else if (strcmp(obis_code, "1.1.4.8.0.255") == 0) {
                float val = (float)obis_value * 0.01f;
                ESP_LOGD("elmatare_mbus", "Reactive Energy: %f", val);
                reactive_energy_sensor->publish_state(val);
              } else {
                ESP_LOGV("elmatare_mbus", "Unknown OBIS %s, value: %d", obis_code, obis_value);
              }
            }
          }
          ESP_LOGV("uart", "%d length received", uart_counter);
          ESP_LOGV("uart", "Message length: %d", uart_message[3]);
          uart_counter = 0;
          uart_message[0] = '\0';
          strcpy(uart_message, "");
        } else {
          uart_counter = 0;
        }
      }
      uart_buffer_[uart_counter] = temp_byte;
      uart_counter++;
    }

    return false;
  }
};
