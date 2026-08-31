Elmätare M-Bus (ESPHome component)

Denna mapp (components/elmatare_mbus) finns för att ESPhome ska kunna ladda komponenten när du använder `external_components` i din YAML.

Placering i repo:
  components/elmatare_mbus/elmatare_mbus.h

Användning
- I din ESPHome YAML (exempel finns i repo-root) behåll `components: [ "elmatare_mbus" ]` i external_components.
- Om ESPhome fortfarande använder cache kan du tvinga uppdatering genom att lägga till `refresh: always` i external_components-posten temporärt.
