IMAGE=quay.io/zagraves/metar-map


all: build

build:
	docker build -t $(IMAGE) .

build-nc:
	docker build --no-cache -t $(IMAGE) .

run: 
	docker run --rm --env-file .env $(IMAGE)