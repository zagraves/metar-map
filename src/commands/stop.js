import lights from '../services/lights';
import data from '../../data/custom.json';

export default async function stop(command) {
  lights.reset(data.lights.length)();
}
