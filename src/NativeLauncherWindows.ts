import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface NativeLaunchResult {
  success: boolean;
  method: string;
  target: string;
  processId?: number;
}

export interface NativeBringToFrontResult {
  success: boolean;
  processId?: number;
  windowTitle?: string;
}

export interface LaunchProcessOptions {
  filePath: string;
  arguments?: string[];
  workingDirectory?: string;
}

export interface LaunchBatchFileOptions {
  filePath: string;
  arguments?: string[];
  workingDirectory?: string;
}

export interface LaunchUriOptions {
  uri: string;
}

export interface WindowSelector {
  processId?: number;
  exactTitle?: string;
  titleContains?: string;
  executableName?: string;
}

export interface Spec extends TurboModule {
  launchProcess(options: LaunchProcessOptions): Promise<NativeLaunchResult>;
  launchBatchFile(options: LaunchBatchFileOptions): Promise<NativeLaunchResult>;
  launchUri(options: LaunchUriOptions): Promise<NativeLaunchResult>;
  bringWindowToFront(
    selector: WindowSelector
  ): Promise<NativeBringToFrontResult>;
}

export default TurboModuleRegistry.get<Spec>('LauncherWindows');
