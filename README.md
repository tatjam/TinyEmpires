# TinyEmpires
#### Age of Empires like game with fantasy elements




## Building

This project is being developed on Visual Studio 2017, and is the easiest platform to build this on.

### Dependencies

[**SFML**](https://www.sfml-dev.org/): Windowing, graphics, audio, network, etc...
[**YAMLcpp**](https://github.com/jbeder/yaml-cpp): YAML parser and writer
[**Boost**](http://www.boost.org/): Used by YAMLcpp

Make sure dependencies are built both for Debug and Release (SFML comes pre-built for both, but yamlcpp requires building).

### Debug Building:

**Link**: ``libyaml-cppmdd.lib;sfml-network-d.lib;sfml-window-d.lib;sfml-system-d.lib;sfml-graphics-d.lib;`` (and all additional dependencies)

**Include**: SFML include, libyaml include and boost include

### Release Building:


**Link**: ``libyaml-cppmd.lib;sfml-network.lib;sfml-system.lib;sfml-window.lib;sfml-graphics.lib;`` (and all additional depencies)

**Include**: SFML include, libyaml include and boost include

### Why boost?

It is required by YAMLcpp, but may soon not be required. If Boost stops being required it will stop being a dependency

(Also, I plan on using either a custom format or JSON if YAMLcpp gets too clumsy)

## Credits

A lot of art is obtained from ``opengameart.org``, check ``CREDITS.txt`` file in ``TinyEmpires/BoardGame/package/res/`` to check who made the art in the game.

(Keep in mind all buildings are self-made, and most other graphics are not copied directly from the original tilesets, but are modified)