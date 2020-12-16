import config from 'config';
import * as math from 'mathjs';
import { sprintf } from 'sprintf-js';

// Categories are ordered in ascending priority. 
const categories = config.get('flight-categories');
const layers = config.get('base-layer-codes');

export function get(metar) {
  const search = (vis, base) => (cat) => math.evaluate(cat.expression.map(exp => sprintf(exp, vis, base))).some(Boolean);
  const clouds = metar.clouds.filter(cloud => layers.includes(cloud.code));

  // Find lowest cloud bases (bkn,ovc,vv) below 12000 AGL
  const bases = clouds.length ? Math.min(...clouds.map(cloud => cloud.base_feet_agl)) : 12000;
  const visibility = metar.visibility.miles_float;
  return categories.filter(search(visibility, bases)).slice(-1).shift();
}

export default { get };
