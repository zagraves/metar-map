import config from 'config';
import URL from 'url';
import axios from 'axios';
import parser from 'aewx-metar-parser';
import cheerio from 'cheerio';
import { sprintf } from 'sprintf-js';

const source = config.get('metar-source');

async function get(stations) {
  const codes = stations.map(it => it.toUpperCase());
  const url = URL.resolve(source.host, sprintf(source.path, codes));

  // We'll scrape html, instead of using a more specific API. 
  // This should allow swapping the source url/path in the config
  // to anything else that might output raw METAR strings in the future.
  const response = await axios.get(url, { headers: source.headers });
  const $ = cheerio.load(response.data);

  // Find the first matching metar for the given station.
  // 1. Use the station id to match the desired string. 
  // e.g. "KRNT 151853Z 15006KT 2 1/2SM -RA ...".match(/^KRNT/)
  // 2. Change the selector in `config/default.json` to update 
  // how it queries the document for the metar string. 
  const scrape = (stationId) => 
    $(source.selector).filter((i, el) => $(el).text().match(stationId)).first();

  return codes
    .map((id) => {
      const element = scrape(id);
      
      // Skip missing result
      if (!element.text()) return false;

      const metar = element.text();
      const decoded = parser(metar);

      return { id, metar, decoded };
    })
    .filter(Boolean);
}



export default { get };
