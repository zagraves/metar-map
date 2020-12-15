import lights from "../services/lights";
import airports from '../../data/airports.json';

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

function render(offset) {
  return () => {
    const sequence = new Array(leds.length);

    for (var index = 0; index < sequence.length; index++) {
      const rgb = colorwheel((offset + index) % 256);
      sequence[index] = { rgb, metadata: {} };
    };

    offset = (offset + 1) % 256;

    render(sequence);
  }
}

export default function test(command, options) {
  return new Promise((resolve) => {
    const interval = setInterval(render(0), 1000 / 30);
    const stop = () => clearInterval(interval);
    const reset = [stop, lights.reset, resolve];

    setTimeout(() => reset.map(fn => fn()), command.seconds * 1000);
  })
}
