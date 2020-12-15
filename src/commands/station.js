import stations from '../services/station';

export default async function station(station, options) {
  const metar = await stations.get(station);
  process.stdout.write(JSON.stringify(metar));
}