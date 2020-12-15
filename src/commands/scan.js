import debug from 'debug';
import Promise from 'bluebird';
import stations from '../services/station';
import lights from '../services/lights';
import data from '../../data/custom.json';

export default async function scan(command) {
  const sequence = new Array(data.airports.length);
  const length = data.lights.length;

  await Promise.all(data.airports.map(async (airport) => {
    const index = airport.light;
    const station = await stations.get(airport.station);
    const rgb = station.colors;
    const metadata = { icon: station.icon }

    sequence[index] = { rgb, metadata };

    debug('metar:scan')(`Station ${station.decoded.icao}(${index}) is ${station.icon}: ${station.category}`)

    lights.render(length)(sequence);
  }));
}
