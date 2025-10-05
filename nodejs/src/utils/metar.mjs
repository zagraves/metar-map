import axios from 'axios';
import parser from 'aewx-metar-parser';
import format from 'sprintf-js';
import config from 'config';

const source = config.get('metar-source');

async function get(stations) {
  const codes = stations.map(it => it.toUpperCase());
  const url = format.sprintf(source.uri, codes);

  const response = await axios.get(url, { headers: source.headers });
  const metarLines = response.data.split('\n').filter(Boolean);

  console.log(metarLines);

  return metarLines.map((metar) => {
    const decoded = parse(metar);
    const id = decoded.icao || metar.split(' ')[1];

    return {id, metar, decoded}
  });
}

export function parse(metar) {
  return parser(metar);
}

export default { get, parse };
