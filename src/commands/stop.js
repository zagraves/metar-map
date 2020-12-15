import lights from '../services/lights';

export default async function stop(command) {
  lights.reset({ length });
}
