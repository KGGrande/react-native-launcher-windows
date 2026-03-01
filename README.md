# react-native-launcher-windows

React Native Windows native module for launching external applications, executables, batch scripts, and URI targets.

This package is Windows-only. It exposes a TurboModule-backed API for starting desktop processes and bringing companion app windows forward from a React Native app.

## Installation

```sh
npm install react-native-launcher-windows
```

## Usage

```js
import {
  bringWindowToFront,
  launchBatchFile,
  launchProcess,
  launchUri,
} from 'react-native-launcher-windows';

await launchProcess({
  filePath: 'C:\\Tools\\Helper.exe',
  arguments: ['--silent'],
  workingDirectory: 'C:\\Tools',
});

await launchBatchFile({
  filePath: 'C:\\Scripts\\helper.bat',
  arguments: ['sync'],
});

await launchUri({
  uri: 'https://reactnative.dev',
});

await bringWindowToFront({
  executableName: 'Helper.exe',
});
```

## API

### `launchProcess(options)`

Launches an executable with optional arguments and an optional working directory.

```ts
await launchProcess({
  filePath: 'C:\\Program Files\\MyApp\\MyApp.exe',
  arguments: ['--mode', 'desktop'],
  workingDirectory: 'C:\\Program Files\\MyApp',
});
```

### `launchBatchFile(options)`

Launches a `.bat` or `.cmd` file through `cmd.exe`.

```ts
await launchBatchFile({
  filePath: 'C:\\Automation\\run-task.bat',
  arguments: ['nightly'],
});
```

### `launchUri(options)`

Opens a URI target using the Windows shell.

```ts
await launchUri({
  uri: 'my-helper-app://open/settings',
});
```

### `bringWindowToFront(selector)`

Finds a visible top-level window and tries to restore/foreground it. Matching precedence is:

1. `processId`
2. `exactTitle`
3. `titleContains`
4. `executableName`

```ts
await bringWindowToFront({
  titleContains: 'Companion App',
});
```

## Return Types

Launch methods resolve to:

```ts
type LaunchResult = {
  success: true;
  method: 'process' | 'batch' | 'uri';
  target: string;
  processId: number | null;
};
```

`bringWindowToFront` resolves to:

```ts
type BringToFrontResult = {
  success: boolean;
  processId: number | null;
  windowTitle: string | null;
};
```

## Limitations

- Windows only
- No stdout or stderr capture
- No wait-for-exit or exit code reporting
- `bringWindowToFront` only searches visible top-level windows
- Window activation can still be limited by Windows foreground restrictions

## Manual Testing

- Run the example app on Windows and use the sample screen in [example/src/App.tsx](example/src/App.tsx).
- Validate an `.exe` launch with arguments and working directory.
- Validate a `.bat` launch through `cmd.exe`.
- Validate a standard URL and any registered custom URI scheme.
- Validate window activation by process ID, exact title, partial title, and executable name.

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT

---

Made with [create-react-native-library](https://github.com/callstack/react-native-builder-bob)
