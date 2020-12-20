import debug from 'debug';
import selectn from 'selectn';
import config from 'config';
import stations from '../services/station';
import lights from '../services/lights';
import data from '../../data/custom.json';

export default async function scan(command) {
  const metars = await stations.get(data.airports.map(it => it.station));

  const getMetar = id => metars.find(it => it.id === id);
  const getColor = metar => selectn('category.colors', metar);
  const getIcon = metar => selectn('category.icon', metar);

  const defaultColor = getColor(config.get('fault'));

  const sequence = new Array(data.leds.length);
  sequence.fill({ rgb: defaultColor }, 0);

  // lights.render(data.leds.length)(sequence);

  data.airports.forEach((airport) => {
    const metar = getMetar(airport.station);

    if (metar) {
      const icon = getIcon(metar);
      const rgb = getColor(metar);
      const metadata = { icon, name: airport.station };

      debug('metar:scan')(`Station ${airport.station}(${airport.light}) is ${metar.category.name}: ${icon}`);

      const index = airport.light - 1;
      sequence.splice(index, 1, { rgb, metadata });
    } else {
      debug('metar:scan')(`Station ${airport.station}(${airport.light}) is missing METARs.`)
    }
  });

  console.log(sequence);

  return lights.render(data.leds.length)(sequence);
}
