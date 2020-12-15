import Metar from '../utils/metar';
import categories from '../utils/flight-category'

export async function get(station) {
  const metar = await Metar.get(station);
  const category = categories.get(metar.decoded);

  return { ...metar, ...category };
}

export default { get };
