import config from 'config';
import * as math from 'mathjs';
import format from 'sprintf-js';

// Categories are ordered in ascending priority. 
// All categories have math expressions that are 
// handled as this `OR` that to determine a match.
const categories = config.get('flight-categories');
const baseLayers = config.get('base-layer-codes');

export function get(metar) {
  const search = (context) => 
    (cat) => math.evaluate(cat.expression.map(exp => format.sprintf(exp, context))).some(Boolean);

  // Find lowest cloud bases (bkn,ovc,vv) below 12000 AGL
  const clouds = (metar.clouds || [])
    .filter(cloud => baseLayers.includes(cloud.code))
    .map(cloud => cloud.base_feet_agl);

  const cloud_base = clouds.length ? Math.min(...clouds) : 12000;
  const visibility = metar.visibility.miles_float;

  // Return first matching category.
  return categories.filter(search({ visibility, cloud_base })).slice(-1).shift();
}

export default { get };
