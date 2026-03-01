import { Platform } from 'react-native';

import NativeLauncherWindows, {
  type LaunchBatchFileOptions,
  type LaunchProcessOptions,
  type LaunchUriOptions,
  type NativeBringToFrontResult,
  type NativeLaunchResult,
  type Spec,
  type WindowSelector,
} from './NativeLauncherWindows';

export type LaunchMethod = 'process' | 'batch' | 'uri';

export interface LaunchResult {
  success: true;
  method: LaunchMethod;
  target: string;
  processId: number | null;
}

export interface BringToFrontResult {
  success: boolean;
  processId: number | null;
  windowTitle: string | null;
}

export type {
  LaunchBatchFileOptions,
  LaunchProcessOptions,
  LaunchUriOptions,
  WindowSelector,
};

function requireNonEmptyString(value: unknown, name: string): string {
  if (typeof value !== 'string' || value.trim().length === 0) {
    throw new Error(`${name} must be a non-empty string.`);
  }

  return value;
}

function validateArguments(
  value: LaunchProcessOptions['arguments'],
  name = 'arguments'
): string[] | undefined {
  if (value == null) {
    return undefined;
  }

  if (!Array.isArray(value)) {
    throw new Error(`${name} must be an array of strings when provided.`);
  }

  const invalidItem = value.find((item) => typeof item !== 'string');
  if (invalidItem !== undefined) {
    throw new Error(`${name} must be an array of strings when provided.`);
  }

  return value;
}

function getNativeModule(): Spec {
  if (Platform.OS !== 'windows') {
    throw new Error(
      'react-native-launcher-windows is only supported on Windows.'
    );
  }

  if (!NativeLauncherWindows) {
    throw new Error(
      'LauncherWindows native module is not available. Make sure react-native-launcher-windows is linked on Windows.'
    );
  }

  return NativeLauncherWindows;
}

function normalizeLaunchMethod(method: string): LaunchMethod {
  if (method === 'process' || method === 'batch' || method === 'uri') {
    return method;
  }

  throw new Error(
    `Native module returned an unsupported launch method: ${method}`
  );
}

function normalizeLaunchResult(result: NativeLaunchResult): LaunchResult {
  return {
    success: true,
    method: normalizeLaunchMethod(result.method),
    target: result.target,
    processId: result.processId ?? null,
  };
}

function normalizeBringToFrontResult(
  result: NativeBringToFrontResult
): BringToFrontResult {
  return {
    success: result.success,
    processId: result.processId ?? null,
    windowTitle: result.windowTitle ?? null,
  };
}

function validateLaunchProcessOptions(
  options: LaunchProcessOptions
): LaunchProcessOptions {
  const filePath = requireNonEmptyString(options?.filePath, 'filePath');
  const args = validateArguments(options.arguments);
  const workingDirectory =
    options.workingDirectory == null
      ? undefined
      : requireNonEmptyString(options.workingDirectory, 'workingDirectory');

  return {
    filePath,
    ...(args ? { arguments: args } : {}),
    ...(workingDirectory ? { workingDirectory } : {}),
  };
}

function validateLaunchBatchFileOptions(
  options: LaunchBatchFileOptions
): LaunchBatchFileOptions {
  const filePath = requireNonEmptyString(options?.filePath, 'filePath');
  const args = validateArguments(options.arguments);
  const workingDirectory =
    options.workingDirectory == null
      ? undefined
      : requireNonEmptyString(options.workingDirectory, 'workingDirectory');

  return {
    filePath,
    ...(args ? { arguments: args } : {}),
    ...(workingDirectory ? { workingDirectory } : {}),
  };
}

function validateLaunchUriOptions(options: LaunchUriOptions): LaunchUriOptions {
  return {
    uri: requireNonEmptyString(options?.uri, 'uri'),
  };
}

function validateWindowSelector(selector: WindowSelector): WindowSelector {
  if (selector == null || typeof selector !== 'object') {
    throw new Error(
      'Window selector must provide processId, exactTitle, titleContains, or executableName.'
    );
  }

  const validated: WindowSelector = {};

  if (selector.processId != null) {
    if (
      typeof selector.processId !== 'number' ||
      !Number.isFinite(selector.processId) ||
      selector.processId <= 0
    ) {
      throw new Error('processId must be a positive number when provided.');
    }

    validated.processId = selector.processId;
  }

  if (selector.exactTitle != null) {
    validated.exactTitle = requireNonEmptyString(
      selector.exactTitle,
      'exactTitle'
    );
  }

  if (selector.titleContains != null) {
    validated.titleContains = requireNonEmptyString(
      selector.titleContains,
      'titleContains'
    );
  }

  if (selector.executableName != null) {
    validated.executableName = requireNonEmptyString(
      selector.executableName,
      'executableName'
    );
  }

  if (
    validated.processId == null &&
    validated.exactTitle == null &&
    validated.titleContains == null &&
    validated.executableName == null
  ) {
    throw new Error(
      'Window selector must provide processId, exactTitle, titleContains, or executableName.'
    );
  }

  return validated;
}

export async function launchProcess(
  options: LaunchProcessOptions
): Promise<LaunchResult> {
  return normalizeLaunchResult(
    await getNativeModule().launchProcess(validateLaunchProcessOptions(options))
  );
}

export async function launchBatchFile(
  options: LaunchBatchFileOptions
): Promise<LaunchResult> {
  return normalizeLaunchResult(
    await getNativeModule().launchBatchFile(
      validateLaunchBatchFileOptions(options)
    )
  );
}

export async function launchUri(
  options: LaunchUriOptions
): Promise<LaunchResult> {
  return normalizeLaunchResult(
    await getNativeModule().launchUri(validateLaunchUriOptions(options))
  );
}

export async function bringWindowToFront(
  selector: WindowSelector
): Promise<BringToFrontResult> {
  return normalizeBringToFrontResult(
    await getNativeModule().bringWindowToFront(validateWindowSelector(selector))
  );
}
