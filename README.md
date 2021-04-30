<img src="https://user-images.githubusercontent.com/17771/102396947-1250ea80-3f92-11eb-932d-e39cea4a68bd.png">


# metar-map

A work-in-progress to transform my VFR sectional chart for the Seattle area into a framed "Metar Map" using a Raspberry Pi Zero W and WS2811 LEDs.

This is my student pilot chart from 2019 that has my solo cross-country, stage check, and checkride routes still plotted out. (e.g. Look for the line plotted over the Whidbey Class C.) I'll be adding LED lights under the map, inside the underlying foam board to light up the various airports in the region with their current flight category. (VFR 🟢, MVFR 🔵, IFR 🔴, LIFR 🟣) 

<img src="https://user-images.githubusercontent.com/17771/102724746-b7d3c900-42c6-11eb-8ee6-9a5205647d74.JPG" width="600">

<img src="https://user-images.githubusercontent.com/17771/102724740-b0142480-42c6-11eb-8345-8b63c880553c.jpeg" width="600">

## Planned layout 

<img src="https://user-images.githubusercontent.com/17771/102383357-491e0500-3f80-11eb-9f66-0b21ba993016.PNG" width="500">

## Configuration

You can customize the airport data and it's respective LED light assignment in `/data/custom.json`.

## Parts

* Custom Frame (BLKSPR/PM918): $110.72*  (frame, acrylic, foam core, +shipping)
* Used VFR Sectional: $free
* Raspberry Pi Zero W: $10.00
* Zebra Zero case - Laser Lime (GPIO): $6.95
* Sandisk 16GB MicroSD card: $6.19
* ALITOVE WS2811 LED Pixels 50pc: $19.99
* Adafruit Quad Level-Shifter: $5.49
* Adafruit USB Micro-B Breakout Board [ADA1833]: $6.43
* 5k Resistor: $free
* 220uF 25V capacitor: $free
* ElectroCookie Mini PCB Prototype Board (6 Pack, Multicolor): $9.99 (1 needed)

### Custom frame 

From `pictureframes.com`
Frame: BLKSPR( 21 3/4" x 21 3/4" )
Approximate Opening Size: 18” x 18”
Borders: L=2", R=2, T=2", B=2"
Mat: PM918 Very White Mat: PM918
Finishing and Backing Options: 1/16" Non-Glare Acrylic with Foamcore Backing (SKU: NGF)

## Wiring

Something like this to safely power the Pi and the LEDs w/ 5v using a 74AHCT125 level shifter. They say don't wire the lights directly off the Pi, so...

<img src="https://cdn-learn.adafruit.com/assets/assets/000/064/121/medium640/led_strips_raspi_NeoPixel_Level_Shifted_bb.jpg?1540314807" width="400">

<img src="https://user-images.githubusercontent.com/17771/102724741-b2767e80-42c6-11eb-9094-2df91cf82ddf.jpeg" width="600">

## Install

Update:

```bash
sudo apt-get update && sudo apt-get upgrade
```

Install dependencies

```bash
sudo apt-get install -y git jq
```

Install NodeJS. We are using v11.15.0 for the ARMv6 architecture on the Pi Zero W in this case.

```
wget https://nodejs.org/download/release/v11.15.0/node-v11.15.0-linux-armv6l.tar.gz
tar -xzf node-v11.15.0-linux-armv6l.tar.gz
sudo cp -r node-v11.15.0-linux-armv6l/* /usr/local/
```

```
git clone https://github.com/zagraves/metar-map.git
cd metar-map
make && make install
```

## Usage

```js
npm start scan
```

Run a 10 second color test

```js
npm start -- -s 10
```

## cron

```sh
# Every 5th minute, start a scan.
*/5 * * * *     sudo /home/pi/metar-map/start.sh 2>&1 | /usr/bin/logger -t metarmap
# At 03:00 on Sunday, run a software update (pull latest code from github)
0 3 * * sun     sudo /home/pi/metar-map/upgrade.sh 2>&1 | /usr/bin/logger -t metarmap

```

## systemctl

```sh
[Unit]
Description=metar-map
Requires=network-online.target
After=network-online.target

[Service]
WorkingDirectory=/home/pi/metar-map
ExecStartPre=-/home/pi/metar-map/test.sh 2>&1 | /usr/bin/logger -t metarmap
ExecStart=/home/pi/metar-map/start.sh 2>&1 | /usr/bin/logger -t metarmap
ExecStop=/home/pi/metar-map/stop.sh 2>&1 | /usr/bin/logger -t metarmap

[Install]
WantedBy=default.target

```

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
        "%(visibility)d > 5",
        "%(cloud_base) > 3000"
      ]
    }
  }
]
```

## Pi Setup:

1. Download Rasbian Buster lite
2. Use balena.io/etcher to flash a compatible microSD card with the disk image. 
3. Enable ssh: https://desertbot.io/blog/ssh-into-pi-zero-over-usb
4. Configure wi-fi (required): `vi /Volumes/boot/etc/wpa_supplicant/wpa_supplicant.conf`

```
# Add to end of file.
network={
    ssid="Your SSID"
    psk="YourWPAPassword"
    key_mgmt=WPA-PSK
}
```

Now configure the Pi:

1. Insert the SD card and connect the Pi to your computer on the *USB* connection only.
2. Connect to Pi: `ssh pi@raspberrypi.local` (default password is: `raspberry`)
3. Run: `sudo apt-get update && sudo apt-get upgrade`
4. Run: `sudo apt-get install -y git nodejs npm jq`


Now, disconnect the Pi and get it wired into the frame.

### Credits:

A thank you to:

* fboes for `aewx-metar-parser`, an excellent METAR parsing library for nodejs. 
* prueker for his own metar map implementation that I drew a lot from.
