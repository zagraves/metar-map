import Metar from '../utils/metar';
import categories from '../utils/flight-category'

export async function get(station) {
  const { metar, decoded } = await Metar.get(station);
  const { category, icon } = categories.get(decoded);

  return { category, icon, metar, decoded };
}

export default { get };
