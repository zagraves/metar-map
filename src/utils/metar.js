import config from 'config';
import URL from 'url';
import axios from 'axios';
import parser from 'aewx-metar-parser';
import cheerio from 'cheerio';

const source = config.get('metar-source');

async function get(station) {
  const stationCode = station.toUpperCase();
  
  const path = `/metar/data?ids=${stationCode}&format=${source.format}&date=&hours=0`;
  const url = URL.resolve(source.host, path);

  const response = await axios.get(url);
  const $ = cheerio.load(response.data);
  const metar = $(source.selector).text();
  const decoded = parser(metar);

  if (!metar) {
    throw new Error('Could not fetch/parse weather');
  }

  return { metar, decoded };
}

export default { get };
