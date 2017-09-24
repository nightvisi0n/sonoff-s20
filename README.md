# (yet another) firmware for the [sonoff s20 smart socket](http://sonoff.itead.cc/en/products/residential/s20-socket) #

## Features ##

1.  [platformio](http://platformio.org/) support
1.  MQTT controllable
1.  [OTA capable](http://docs.platformio.org/en/latest/platforms/espressif8266.html?highlight=ota#over-the-air-ota-update)
1.  easy configuration through gcc preprocessor `#define`s

## Configuration ##

Have a look at [src/config.h](src/config.h) and adapt it to your needs.

## Compilation ##

```shell
~/git/sonoff-s20/ › pio run
```

### Upload (via USB to Serial Adapter) ###

```shell
~/git/sonoff-s20/ › pio run -t upload
```

### Upload (via OTA) ###

First change the OTA password in [platformio.ini](platformio.ini) if you modified the default one in [src/config.h](src/config.h).

```shell
~/git/sonoff-s20/ › pio run -t upload --upload-port=<esp8266 ip address>
```

## License ##

Copyright (C) 2017 nightvisi0n

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
