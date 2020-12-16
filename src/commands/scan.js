import debug from 'debug';
import selectn from 'selectn';
import stations from '../services/station';
import lights from '../services/lights';
import data from '../../data/custom.json';

export default async function scan(command) {
  const { leds, fault } = data; 
  const metars = await stations.get(data.airports.map(it => it.station));

  const getMetar = id => metars.find(it => it.id === id);
  const getColor = metar => selectn('category.colors', metar);
  const getIcon = metar => selectn('category.icon', metar);

  const sequence = new Array(data.airports.length);
  sequence.fill({ rgb: getColor(fault) }, 0);

  data.airports.forEach((airport) => {
    const metar = getMetar(airport.station);

    if (metar) {
      const icon = getIcon(metar);
      const rgb = getColor(metar);
      const metadata = { icon, name: airport.station };

      debug('metar:scan')(`Station ${airport.station}(${airport.light}) is ${metar.category.name}: ${icon}`)

      sequence.splice(airport.light, 1, { rgb, metadata });
    }
  });

  return lights.render(leds.length)(sequence);
}
