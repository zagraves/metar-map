import config from 'config';
import URL from 'url';
import axios from 'axios';
import parser from 'aewx-metar-parser';
import cheerio from 'cheerio';

const source = config.get('metar-source');

async function get(stations) {
  const codes = stations.map(s => s.toUpperCase());
  const path = `/metar/data?ids=${codes}&format=${source.format}&date=&hours=0`;
  const url = URL.resolve(source.host, path);

  const response = await axios.get(url);
  const $ = cheerio.load(response.data);

  return codes.map((id) => {
    const element = $(source.selector)
      .filter((i, el) => $(el).text().match(id))
      .first();
      
    if (!element.text()) return false;

    const metar = element.text();
    const decoded = parser(metar);

    return { id, metar, decoded };
  }).filter(Boolean);
}



export default { get };
