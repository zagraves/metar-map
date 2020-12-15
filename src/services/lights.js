import ws281x from 'rpi-ws281x-native';
import lights from '../../data/lights.json';

ws281x.init(lights.length);

// https://github.com/beyondscreen/node-rpi-ws281x-native

export function render() {
  const pixelData = new Uint32Array(lights.length);

  return ws281x.render(pixelData);
}