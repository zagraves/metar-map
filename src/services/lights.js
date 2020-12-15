import debug from 'debug';
import ws281x from 'rpi-ws281x-native';
import rgb from '../utils/rgb'
import leds from '../../data/leds.json';

// https://github.com/beyondscreen/node-rpi-ws281x-native

ws281x.init(leds.length);

export function render(colors) {
  const pixels = colors
    .reduce((acc, color, index) => {
      acc.set([rgb.toInt(...color.rgb)], index);

      debug('metar:lights')(`Light #${index}: rgb(${color.rgb}) ${JSON.stringify(color.metadata)}`);

      return acc;
    }, new Uint32Array(leds.length));

  debug('metar:render')(`Rendering lights: [${pixels}]`)

  // I hope this wont flicker.
  return ws281x.render(pixels);
}

export function reset() {
  debug('metar:lights')('Resetting lights...')
  ws281x.reset();
}

// ws281x.setBrightness

export default { render, reset };
