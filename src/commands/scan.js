import debug from 'debug';
import Promise from 'bluebird';
import stations from '../services/station';
import lights from '../services/lights';
import airports from '../../data/airports.json';

export default async function scan(command) {
  const sequence = new Array(airports.length);

  await Promise.all(airports.map(async (airport) => {
    const index = airport.light;
    const station = await stations.get(airport.station);
    const rgb = station.colors;
    const metadata = { icon: station.icon }

    sequence[index] = { rgb, metadata };

    debug('metar:scan')(`Station ${station.decoded.icao}(${index}) is ${station.icon}: ${station.category}`)

    lights.render(sequence, { length });
  }));
}
