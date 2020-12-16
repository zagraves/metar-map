# metar-map

A work-in-progress to transform my VFR sectional chart for the Seattle area into a framed "Metar Map" using a Raspberry Pi Zero W and WS2811 LEDs.

This is my student pilot chart from 2019 that has my XC routes still plotted out. (Look for the line plotted over the Whidbey Class C) I'll be adding LED lights under the map, inside the underlying foam board to light up the various airports in the region with their current flight category. (VFR 🟢, MVFR 🔵, IFR 🔴, LIFR 🟣) The output of this project is a Docker image that can be easily run by a RPi by scheduling a cron and/or using systemctl.

<img src="https://user-images.githubusercontent.com/17771/102384271-60a9bd80-3f81-11eb-8117-7f47dda3ae0f.jpg" width="500">

## Planned layout 

<img src="https://user-images.githubusercontent.com/17771/102383357-491e0500-3f80-11eb-9f66-0b21ba993016.PNG" width="500">

## Configuration

You can customize the airport data and it's respective LED light assignment in `/data/custom.json`.

## Parts

* Rpi Zero W
* Zebra Zero for Raspberry Pi Zero & Zero Wireless case - Laser Lime w/GPIO
* Adafruit USB Micro-B Breakout Board [ADA1833] (To split the 5v input to the lights and the Pi)
* Adafruit Accessories Quad Level-Shifter (74AHCT125)
* ALITOVE WS2811 addressable leds.
* Jumper wires
* SD card

## Wiring

Something like this to safely power the Pi and the LEDs w/ 5v using a 74AHCT125 level shifter. They say don't wire the lights directly off the Pi, so...

<img src="https://cdn-learn.adafruit.com/assets/assets/000/064/121/medium640/led_strips_raspi_NeoPixel_Level_Shifted_bb.jpg?1540314807" width="400">

## Usage

```js
docker run --rm quay.io/zagraves/metar-map:latest scan
```

Run a 10 second color test

```js
docker run --rm quay.io/zagraves/metar-map:latest scan -- -s 10
```

## cron

```
# At every 10th minute past every hour from 6 through 22, scan the airports and update the lights.
*/10 6-22 * * *   docker run --rm -v /home/pi/metar-map/config.json:/app/data/custom.json quay.io/zagraves/metar-map:latest scan
```

```
# At 22:55, turn off the lights.
55 22 * * *       docker run --rm -v /home/pi/metar-map/config.json:/app/data/custom.json quay.io/zagraves/metar-map:latest stop
```

## systemctl

See metar.service for the systemctl config, which pulls the latest image, runs a 2s color test followed by a scan immediately after startup of the device.

## CLI

For debugging... Find the current metar for `KRNT` and pipe the output to jq.

```
npm --silent start station krnt | jq

[
  {
    "id": "KRNT",
    "metar": "KRNT 161653Z 16007KT 10SM OVC048 08/06 A2999 RMK AO2 SLP160 T00780056",
    "decoded": {
      "raw_text": "KRNT 161653Z 16007KT 10SM OVC048 08/06 A2999 RMK AO2 SLP160 T00780056",
      "raw_parts": [
        "KRNT",
        "161653Z",
        "16007KT",
        "10SM",
        "OVC048",
        "08/06",
        "A2999",
        "RMK",
        "AO2",
        "SLP160",
        "T00780056"
      ],
      "icao": "KRNT",
      "observed": "2020-12-16T16:53:40.651Z",
      "wind": {
        "degrees": 160,
        "speed_kts": 7,
        "speed_mps": 3.60111109710679,
        "gust_kts": 7,
        "gust_mps": 3.60111109710679
      },
      "visibility": {
        "miles": "10",
        "miles_float": 10,
        "meters": "16000",
        "meters_float": 16093.44
      },
      "clouds": [
        {
          "code": "OVC",
          "base_feet_agl": 4800,
          "base_meters_agl": 1463.04
        }
      ],
      "temperature": {
        "celsius": 8,
        "fahrenheit": 46.4
      },
      "dewpoint": {
        "celsius": 6,
        "fahrenheit": 42.8
      },
      "humidity_percent": 87.19415400994988,
      "barometer": {
        "hg": 29.99,
        "kpa": 101.55777916333727,
        "mb": 1015.5777916333727
      },
      "ceiling": {
        "code": "OVC",
        "feet_agl": 4800,
        "meters_agl": 1463.04
      },
      "flight_category": "VFR"
    },
    "category": {
      "name": "VFR",
      "icon": "🟢",
      "colors": [
        0,
        255,
        0
      ],
      "expression": [
        "%1$d > 5",
        "%2$d > 3000"
      ]
    }
  }
]
```

### Credits:

A thank you to:

* fboes for `aewx-metar-parser`, an excellent METAR parsing library for nodejs. 
* prueker for his own metar map implementation that I drew a lot from.
