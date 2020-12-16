import metars from '../utils/metar';
import categories from '../utils/flight-category'
import station from '../commands/station';

export async function get(ids) {
  const stations = await metars.get(ids);

  return stations.reduce((acc, item) => {
    const category = categories.get(item.decoded);
    const merged = { ...item, category };
    return acc.concat(merged);
  }, []);
}

export function parse(str) {
  return metars.parse(str);
}

export default { get, parse };
