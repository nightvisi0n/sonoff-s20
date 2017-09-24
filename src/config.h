// Copyright (C) 2017 nightvisi0n
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// can be either a ip address or a FQDN (if you have a matching dns entry)
#define MQTT_SERVER         "mqtt.local"
// the mqtt standard port is 1883. normally there is no need to modify this
#define MQTT_PORT           1883
// the user which is used for authenticating at the mqtt server
#define MQTT_USER           "sonoff"
// the password which is used for authenticating at the mqtt server
#define MQTT_PASSWORD       "password"
// codetermines the mqtt topic.
// the whole topic has the following syntax:
// <MQTT_PREFIX>/<ESP8266 chip id>/{command,state,+}
#define MQTT_PREFIX         "sonoff"

// the SSID of the wifi to connect to
#define WIFI_SSID           "iot"
// the matching password
#define WIFI_PASSWORD       "password"

// codetermines the hostname (<ESP8266 chip id>.<FQDN_SUFFIX>)
#define FQDN_SUFFIX         "iot.local"

// the password for authenticating before OTA firmware updates
#define OTA_PASSWORD        "password"
