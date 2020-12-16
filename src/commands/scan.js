import debug from 'debug';
import Promise from 'bluebird';
import stations from '../services/station';
import lights from '../services/lights';
import data from '../../data/custom.json';

export default async function scan(command) {
  const { length, fault } = data.lights;
  const sequence = new Array(data.airports.length);

  await Promise.all(data.airports.map(async (airport) => {
    const index = airport.light;

    try {
      const station = await stations.get(airport.station);
      const rgb = station.colors;
      const metadata = { icon: station.icon };

      debug('metar:scan')(`Station ${station.decoded.icao}(${index}) is ${station.icon}: ${station.category}`)

      sequence[index] = { rgb, metadata };
    } catch (err) {
      const { rgb, icon } = fault;
      sequence[index] = { rgb, metadata: { icon } };      
    }

    lights.render(length)(sequence);
  }));
}
