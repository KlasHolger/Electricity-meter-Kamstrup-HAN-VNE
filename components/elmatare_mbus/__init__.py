"""ESPHome external component package for elmatare_mbus.

This minimal __init__.py exists so ESPHome can recognize the component when referenced
via `external_components` in YAML. The actual C++ header lives in this folder
(elmatare_mbus.h).

No Python platform is provided here — the component is used by including the
C++ header and using `platform: custom` + lambda as in the repository example.
"""

# Tell ESPHome there's nothing to auto-load as a Python platform.
AUTO_LOAD = []

# Add yourself as codeowner so maintainers can be contacted easily.
CODEOWNERS = ["@KlasHolger"]
