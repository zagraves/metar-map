import chalk from 'chalk';
import config from 'config';
import Command from 'commander';
import packageJson from '../package.json';

import scan from './commands/scan';
import stop from './commands/stop';
import test from './commands/test';
import station from './commands/station';
import parse from './commands/parse';

import lights from './services/lights';
import data from '../data/custom.json';

const Program = new Command.Command('metar-map')
  .version(packageJson.version, '-v, --version')
  .arguments('<command>')
  .allowUnknownOption(false)
  .usage(`${chalk.green('[command]')}`);

let stdin = '';

function configure(handler) {
  return async function commander(...args) {
    const cmd = args.pop();
    const command = { ...config, ...cmd, stdin };
    const handlerArgs = args.concat(command);

    const onError = (err) => {
      console.error(`${chalk.red(err.message)}`);
      console.error(err);
      process.exit(1);
    };

    return handler(...handlerArgs).catch(onError);
  }
}

if (!process.argv[2]) {
  Program.help();
}

Program.command('scan')
  .description('Scan airports and set lighting according to station status')
  .action(configure(scan));

Program.command('stop')
  .description('Turn off lights, useful before shut-down')
  .action(configure(stop));

Program.command('test')
  .description('Test light sequence for 2 seconds, useful on start-up')
  .option('-s, --seconds [seconds]', 'Time to run test', 2)
  .action(configure(test));

Program.command('station <station>')
  .description('Get station weather for debugging')
  .action(configure(station));

Program.command('parse [str]')
  .description('Parse input metar string')
  .action(configure(parse));

if (process.stdin.isTTY) {
  Program.parse(process.argv);
} else {
  process.stdin.on('readable', function() {
    let chunk = this.read();

    if (chunk !== null) {
      stdin += chunk;
    }
  });

  process.stdin.on('end', function() {
    Program.parse(process.argv); 
  });
}

process.on('uncaughtException', function(err) {
  console.log(err)

  const { length, ...options } = data.leds;
  const sequence = new Array(length);

  sequence.fill({ rgb: defaultColor }, 0);
  lights.render(length, options)(sequence);
})
