import axios from 'axios';
import parser from 'aewx-metar-parser';
import format from 'sprintf-js';
import config from 'config';

const source = config.get('metar-source');

async function get(stations) {
  const codes = stations.map(it => it.toUpperCase());
  const url = format.sprintf(source.uri, codes);

  // We'll scrape html, instead of using a more specific API. 
  // This should allow swapping the source url/path in the config
  // to anything else that might output raw METAR strings in the future.
  const response = await axios.get(url, { headers: source.headers });

  const metarLines = response.data.split("\n").filter(Boolean);

  return metarLines.map((metar) => {
    const id = metar.split(' ')[0];
    const decoded = parse(metar);

    return {id, metar, decoded}
  });
}

export function parse(metar) {
  return parser(metar);
}

export default { get, parse };
