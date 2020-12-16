import lights from "../services/lights";
import data from '../../data/custom.json';

function colorwheel(pos) {
  pos = 255 - pos;

  if (pos < 85) {
    return [255 - pos * 3, 0, pos * 3];
  } else if (pos < 170) { 
    pos -= 85; 
    return [0, pos * 3, 255 - pos * 3];
  } else { 
    pos -= 170;
    return [pos * 3, 255 - pos * 3, 0];
  }
}

function render({ offset, length }) {
  return () => {
    const sequence = new Array(length);

    for (var index = 0; index < sequence.length; index++) {
      const rgb = colorwheel((offset + index) % 256);
      sequence[index] = { rgb, metadata: {} };
    };

    offset = (offset + 1) % 256;

    lights.render(length)(sequence);
  }
}

export default function test(command, options) {
  return new Promise((resolve) => {
    const { length } = data.lights;
    const timeout = command.seconds * 1000;
    const interval = setInterval(render({ offset: 0, length}), 1000 / 30);
    const stop = () => clearInterval(interval);
    const clear = () => lights.reset(data.lights.length);
    const resetFn = [stop, clear, resolve];

    setTimeout(() => resetFn.map(f => f()), timeout);
  })
}
