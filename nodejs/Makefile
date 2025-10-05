all: build

build:
	npm install

install:
	/usr/bin/crontab ./crontab
	cd nodejs && sudo cp metar.map.service /etc/systemd/system/metar.map.service
	sudo systemctl enable metar.map.service
	sudo systemctl start metar.map.service

update:
	git pull origin master
	cd nodejs && sudo cp metar.map.service /etc/systemd/system/metar.map.service
	sudo systemctl daemon-reload
