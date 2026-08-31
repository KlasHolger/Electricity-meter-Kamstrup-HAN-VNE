# Elmätare M-Bus (external component)

Detta är en extern komponent-version av den tidigare custom component-koden för att läsa Kamstrup/AMS via HAN-VNE (M-Bus över UART).

Placering i detta repo:

  external_components/elmatare_mbus/elmatare_mbus.h

Hur du använder komponenten i ditt ESPHome-projekt

Alternativ A — använd komponenten direkt från detta GitHub-repo (rekommenderas):

1. Lägg till följande i din ESPHome YAML (byt ut branchnamnet om du vill):

external_components:
  - source: github://KlasHolger/Electricity-meter-Kamstrup-HAN-VNE@External-Component
    components: [ "elmatare_mbus" ]

2. Inkludera header-filen (detta säkerställer att klassen hittas vid kompilering):

includes:
  - external_components/elmatare_mbus/elmatare_mbus.h

3. Använd samma lambda som tidigare för att registrera komponenten och sensorerna:

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

Notera
- Jag har inte ändrat sensornamn/skalning i koden — dessa hanteras i YAML med filters som tidigare.
- Koden i headern har en mindre säkerhetsfix (loopen som letar OBIS-kod startar från index 1) för att undvika index-1 när buffern undersöks.

Om du vill kan jag också:
- Förbättra felhantering och parsing (t.ex. hantera teckenflöden mer robust)
- Flytta koden till en separat repo och skapa en version/tags

