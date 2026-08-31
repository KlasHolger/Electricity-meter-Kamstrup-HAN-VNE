//----------------------------------------------------------------------------
//Filnamna: elmatare_mbus
//Version: elmatare_mbus 1
//tidigare version: mbus_v6.h
//Klas Jansson 2022-03-05
//Initial code from Home Assistant / Hacky integratin for M-Bus
//change to increase readability
//completion for reading of 3-phase AMS
//removal of unnecessary code
//
//----------------------------------------------------------------------------
#include "esphome.h"

class MbusReader : public Component, public uart::UARTDevice, public Sensor {
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

  Sensor *wattage_sensor = new Sensor(); //Aktiv effekt uttag - obis_code 1.1.1.7.0.255
  Sensor *amperageL1_sensor = new Sensor(); //Fasstrom L1 - obis_code 1.1.31.7.0.255
  Sensor *amperageL2_sensor = new Sensor(); //Fasstrom L2 - obis_code 1.1.51.7.0.255
  Sensor *amperageL3_sensor = new Sensor(); //Fasstrom L3 - obis_code 1.1.71.7.0.255
  Sensor *voltageL1_sensor = new Sensor(); //Fasvoltage L1.- obis_code, "1.1.32.7.0.255"
  Sensor *voltageL2_sensor = new Sensor(); //Fasvoltage L1.- obis_code, "1.1.52.7.0.255"
  Sensor *voltageL3_sensor = new Sensor(); //Fasvoltage L1.- obis_code, "1.1.72.7.0.255"
  Sensor *energy_sensor = new Sensor(); //Aktiv energy uttag -. obis_code, "1.1.1.8.0.255"
  Sensor *reactive_power_sensor = new Sensor(); //Reaktiv effekt Inmatning - obis_code, "1.1.4.7.0.255"
  Sensor *reactive_energy_sensor = new Sensor(); //Mätarställning Rektiv Energi Inmatning - obis_code, "1.1.4.8.0.255"

  void setup() override {

  }

  void loop() override {
    bool have_message = read_message();
  }

  bool read_message() {
    while(available() >= 1) {
      read_byte(this->temp_byte_pointer);
      if(temp_byte == 126) {
        if(uart_counter > 2) {
          uart_buffer_[uart_counter] = temp_byte;
          uart_counter++;
          uart_message[0] = '\0';
          strcpy(uart_message, "");
          for (uint16_t i = 0; i < uart_counter && i < 256; i++) {
           if(uart_buffer_[i-1] == 9 && uart_buffer_[i] == 6) {
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
              if(uart_buffer_[i + 7] == 6) {
                for(uint8_t y = 0; y < 4; y++) {
                  obis_value += (long)uart_buffer_[i + 8 + y] << ((3-y) * 8);
                }
              } else if(uart_buffer_[i + 7] == 18) {
                for(uint8_t y = 0; y < 2; y++) {
                  obis_value += (long)uart_buffer_[i + 8 + y] << ((1-y) * 8);
                }
              }
               
              //Aktiv effekt uttag
              if(strcmp(obis_code, "1.1.1.7.0.255") == 0) {
                  ESP_LOGV("uart", "Wattage: %d", obis_value);
                  wattage_sensor->publish_state(obis_value);
              } 
                //Fasstrom L1
                else if (strcmp(obis_code, "1.1.31.7.0.255") == 0) {
                  ESP_LOGV("uart", "AmperageL1: %d", obis_value);
                  amperageL1_sensor->publish_state(obis_value);
              } 
                //Fasstrom L2
                else if (strcmp(obis_code, "1.1.51.7.0.255") == 0) {
                  ESP_LOGV("uart", "AmperageL2: %d", obis_value);
                  amperageL2_sensor->publish_state(obis_value);
              } 
                //Fasstrom L3
                else if (strcmp(obis_code, "1.1.71.7.0.255") == 0) {
                  ESP_LOGV("uart", "AmperageL3: %d", obis_value);
                  amperageL3_sensor->publish_state(obis_value);
              } 
                //Fasvoltage L1
                else if (strcmp(obis_code, "1.1.32.7.0.255") == 0) {
                  ESP_LOGV("uart", "Voltage: %d", obis_value);
                  voltageL1_sensor->publish_state(obis_value);
              } 
                //Fasvoltage L2
                else if (strcmp(obis_code, "1.1.52.7.0.255") == 0) {
                  ESP_LOGV("uart", "Voltage: %d", obis_value);
                  voltageL2_sensor->publish_state(obis_value);
              } 
                //Fasvoltage L3
                else if (strcmp(obis_code, "1.1.72.7.0.255") == 0) {
                  ESP_LOGV("uart", "Voltage: %d", obis_value);
                  voltageL3_sensor->publish_state(obis_value);
              } 
                //Aktiv energy uttag
                else if (strcmp(obis_code, "1.1.1.8.0.255") == 0) {
                  ESP_LOGV("uart", "Energy Usage Last Hour: %d", obis_value);
                  energy_sensor->publish_state(obis_value);
              } 
                //Reaktiv effekt Inmatning
                else if (strcmp(obis_code, "1.1.4.7.0.255") == 0) {
                  ESP_LOGV("uart", "Reactive Power: %d", obis_value);
                  reactive_power_sensor->publish_state(obis_value);
              } 
                //Mätarställning Rektiv Energi Inmatning
                else if (strcmp(obis_code, "1.1.4.8.0.255") == 0) {
                  ESP_LOGV("uart", "Reactive Power Last Hour: %d", obis_value);
                  reactive_energy_sensor->publish_state(obis_value);
              } 
                else {
                ESP_LOGV("uart", "Unknown OBIS %s, value: %d", obis_code, obis_value);
              }
            }
            //strncat(uart_message, " ", 1);
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
