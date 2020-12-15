import lights from '../services/lights';

export default async function scan(command) {
  lights.reset();
}