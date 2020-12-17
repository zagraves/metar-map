IMAGE=quay.io/zagraves/metar-map


all: build

build:
	npm install

install:
	# /usr/bin/crontab ./crontab
	echo "install"

update:
	git pull origin master