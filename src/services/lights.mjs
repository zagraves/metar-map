// import Promise from 'bluebird';
import debug from 'debug';
import ws281x from 'rpi-ws281x-native';
import rgb from '../utils/rgb';

// https://github.com/beyondscreen/node-rpi-ws281x-native

export function render(length) {
  return async (colors) => {
    const pixels = colors
      .reduce((acc, color, index) => {
        if (index < length) {
          acc.set([rgb.toInt(...color.rgb)], index);
          debug('metar:lights')(`Light #${index}: rgb(${color.rgb}) ${JSON.stringify(color.metadata)}`);
        }

        return acc;
      }, new Uint32Array(length));

    debug('metar:render')(`Rendering lights: [${pixels}]`)

    ws281x.init(length);
    ws281x.render(pixels);
    ws281x.render(pixels);
  }
}

export function reset(length) {
  debug('metar:lights')('Resetting lights...')
  ws281x.init(length);
  return () => ws281x.reset();
}

// ws281x.setBrightness

export default { render, reset };
