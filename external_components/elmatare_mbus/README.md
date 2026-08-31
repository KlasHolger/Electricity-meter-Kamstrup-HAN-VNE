# Elmätare M-Bus (external component)

Detta är en extern komponent-version av den tidigare custom component-koden för att läsa Kamstrup/AMS via HAN-VNE (M-Bus över UART).

Placering i detta repo:

  external_components/elmatare_mbus/elmatare_mbus.h

Snabb användning i ESPHome-projekt

1. Lägg till följande i din ESPHome YAML (byt ut branchnamnet om du vill):

external_components:
  - source: github://KlasHolger/Electricity-meter-Kamstrup-HAN-VNE@External-Component
    components: [ "elmatare_mbus" ]

2. Inkludera header-filen (detta säkerställer att klassen hittas vid kompilering):

includes:
  - external_components/elmatare_mbus/elmatare_mbus.h

3. Använd följande sensor-konfiguration (ingen multiply-filter behövs eftersom C++ sköter skalning):

sensor:
  - platform: custom
    lambda: |-
      auto mbus_reader = new MbusReader(id(uart_bus));
      App.register_component(mbus_reader);
      return {
        mbus_reader->wattage_sensor,
        mbus_reader->reactive_power_sensor,
        mbus_reader->amperageL1_sensor,
        mbus_reader->amperageL2_sensor,
        mbus_reader->amperageL3_sensor,
        mbus_reader->voltageL1_sensor,
        mbus_reader->voltageL2_sensor,
        mbus_reader->voltageL3_sensor,
        mbus_reader->energy_sensor,
        mbus_reader->reactive_energy_sensor
      };

Sensorerna (fasta namn föreslås i YAML-exemplet i repo):
- "AMS Wattage" (kW)
- "AMS Reactive Power" (VAr)
- "AMS Amperage L1" (A)
- "AMS Amperage L2" (A)
- "AMS Amperage L3" (A)
- "AMS Voltage L1" (V)
- "AMS Voltage L2" (V)
- "AMS Voltage L3" (V)
- "AMS Energy" (kWh)
- "AMS Hourly Reactive Energy" (kVArh)

