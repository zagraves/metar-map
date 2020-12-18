IMAGE=quay.io/zagraves/metar-map


all: build

build:
	npm install

install:
	/usr/bin/crontab ./crontab
	sudo cp metar.map.service /etc/systemd/system/metar.map.service
	sudo systemctl enable metar.map.service
	sudo systemctl daemon-reload

update:
	git pull origin master