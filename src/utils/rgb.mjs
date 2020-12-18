import { blue } from "chalk";

export function rgb(r, g, b) {
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

export function grb(r, g, b) {
  return ((g & 0xff) << 8) ((r & 0xff) << 16) + (b & 0xff);
}

export default { rgb, grb };
