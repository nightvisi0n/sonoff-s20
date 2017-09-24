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

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <Ticker.h>

#include "config.h"

#define PIN_BUTTON          0
#define PIN_RELAY           12
#define PIN_LED             13

// the chip id is the last 6 chars of the mac address
#define ESP8266_CHIPID_SIZE 6

enum state_e {
    STATE_OFF,
    STATE_ON
};
enum state_e state;
enum state_e prev_state;

bool state_request = false;

void callback(char*, byte*, unsigned int);

WiFiClient wifiClient;
PubSubClient mqttClient(MQTT_SERVER, MQTT_PORT, callback, wifiClient);
Ticker btn_timer;

// format:
// <ESP8266_CHIPID><FQDN_SUFFIX>
// the '-1' is subtracting null bytes from intermediate strings
char esp_hostname[
    ESP8266_CHIPID_SIZE
    + sizeof(FQDN_SUFFIX)-1
    + sizeof(".")-1
    + 1 // terminating null byte
];
// format:
// <MQTT_PREFIX>/<ESP8266_CHIPID>/command
char mqtt_topic_command[
    sizeof(MQTT_PREFIX)-1
    + sizeof("/")-1
    + ESP8266_CHIPID_SIZE
    + sizeof("/command")-1
    + 1 // terminating null byte
];
// format:
// <MQTT_PREFIX>/<ESP8266_CHIPID>/state
char mqtt_topic_state[
    sizeof(MQTT_PREFIX)-1
    + sizeof("/")-1
    + ESP8266_CHIPID_SIZE
    + sizeof("/state")-1
    + 1 // terminating null byte
];

void setup() {
    state = STATE_OFF;

    snprintf(
        esp_hostname,
        sizeof(esp_hostname),
        "%x.%s", ESP.getChipId(), FQDN_SUFFIX
    );
    snprintf(
        mqtt_topic_state,
        sizeof(mqtt_topic_state),
        "sonoff/%x/state", ESP.getChipId()
    );
    snprintf(
        mqtt_topic_command,
        sizeof(mqtt_topic_command),
        "sonoff/%x/command", ESP.getChipId()
    );

    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_BUTTON, INPUT);

    digitalWrite(PIN_LED, HIGH);
    digitalWrite(PIN_RELAY, LOW);

    EEPROM.begin(8);
    state = (state_e)EEPROM.read(0);
    prev_state = state;
    state_request = true;

    btn_timer.attach_ms(50, check_button);

    WiFi.hostname(esp_hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    ArduinoOTA.setHostname(esp_hostname);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        if (progress % 100   == 0) {
            digitalWrite(PIN_LED, !digitalRead(PIN_LED));
        }
    });
    ArduinoOTA.onEnd([]() {
        digitalWrite(PIN_LED, HIGH);
        flash_led(500, 2);
    });
    ArduinoOTA.onError([](ota_error_t error) {
        digitalWrite(PIN_LED, HIGH);
        flash_led(200, 5);

        ESP.restart();
    });

    ArduinoOTA.begin();

    while ((WiFi.status() != WL_CONNECTED)) {
        flash_led(30, 1);
        delay(500);
    }

    flash_led(500, 1);
}

void loop() {
    if (!mqttClient.connected()) {
      reconnect();
    }

    ArduinoOTA.handle();
    mqttClient.loop();

    switch (state) {
    case STATE_OFF:
        digitalWrite(PIN_LED, HIGH);
        digitalWrite(PIN_RELAY, LOW);
        break;
    case STATE_ON:
        digitalWrite(PIN_LED, LOW);
        digitalWrite(PIN_RELAY, HIGH);
        break;
    }

    if (prev_state != state) {
        EEPROM.write(0, state);
        EEPROM.commit();
        prev_state = state;
    }

    if (state_request) {
        switch (state) {
        case STATE_ON:
            mqttClient.publish(
                mqtt_topic_state,
                "ON",
                true
            );
            break;
        case STATE_OFF:
            mqttClient.publish(
                mqtt_topic_state,
                "OFF",
                true
            );
            break;
        }
        state_request = false;
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    if (0 == strncmp(topic, mqtt_topic_command, sizeof(mqtt_topic_command))) {
        if (0 == strncmp((char *)payload, "ON", 2)) {
            state = STATE_ON;
        } else if (0 == strncmp((char *)payload, "OFF", 3)) {
            state = STATE_OFF;
        }
        state_request = true;
    }
}

void flash_led(int duration, int n) {
    for(int i = 0; i < n; i++)  {
        digitalWrite(PIN_LED, LOW);
        delay(duration);
        digitalWrite(PIN_LED, HIGH);
        delay(duration);
    }
}

void check_button() {
    static unsigned long count = 0;

    if (!digitalRead(PIN_BUTTON)) {
        count++;
    } else {
        if (count > 1 && count <= 40) {
            switch (state) {
            case STATE_ON:
                state = STATE_OFF;
                break;
            case STATE_OFF:
                state = STATE_ON;
                break;
            }
            state_request = true;
        }
        count = 0;
    }
}

void reconnect() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect(
            esp_hostname,
            MQTT_USER,
            MQTT_PASSWORD
        )) {
            mqttClient.subscribe(mqtt_topic_command);
            flash_led(500, 1);
        } else {
            flash_led(200, 1);
            delay(500);
            mqttClient.disconnect();
        }
    }
}
