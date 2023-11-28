// import Promise from 'bluebird';
import debug from 'debug';
import ws281x from 'rpi-ws281x-native';
import { rgb, grb } from '../utils/rgb';

// https://github.com/beyondscreen/node-rpi-ws281x-native

export function render(length, options) {
  return async (colors) => {
    const pixels = colors
      .reduce((acc, color, index) => {
        if (index < length) {
          acc.set([rgb(...color.rgb)], index);
          debug('metar:lights')(`Light #${index}: rgb(${color.rgb}) ${JSON.stringify(color.metadata)}`);
        }

        return acc;
      }, new Uint32Array(length));

    debug('metar:render')(`Rendering lights: [${pixels}]`)

    ws281x.init(length, options);
    ws281x.render(pixels);
  }
}

export function reset(length) {
  debug('metar:lights')('Resetting lights...')
  ws281x.init(length);
  return () => ws281x.reset();
}

export function setError(length, options) {
  return async () {
    const sequence = new Array(length);
    sequence.fill({ rgb: [255, 255, 0] }, 0); // yellow
  
    return render(length, options)(sequence);
  }
}

// ws281x.setBrightness

export default { render, reset, setError };
