import stations from '../services/station';

export default async function parse(command, options) {
  const metar = await stations.parse(options.stdin);
  process.stdout.write(JSON.stringify(metar));
}