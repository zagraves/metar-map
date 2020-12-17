IMAGE=quay.io/zagraves/metar-map


all: build

build:
	npm install

install:
	# /usr/bin/crontab ./crontab
	echo "install"
	sudo cp metar.map.service /etc/systemd/system/metar.map.service
	sudo systemctl enable metar.map.service

update:
	git pull origin master