import config from 'config';
import URL from 'url';
import axios from 'axios';
import parser from 'aewx-metar-parser';
import cheerio from 'cheerio';

const host = config.get('metar-source.host');
const format = config.get('metar-source.format');

async function get(station) {
  const stationCode = station.toUpperCase();
  const url = URL.resolve(host, `/metar/data?ids=${stationCode}&format=${format}&date=&hours=0`);

  const response = await axios.get(url);
  const $ = cheerio.load(response.data);
  const metar = $('#awc_main_content code').text();
  const decoded = parser(metar);

  if (!metar) {
    throw new Error('Could not fetch/parse weather');
  }

  return { metar, decoded };
}

export default { get };
