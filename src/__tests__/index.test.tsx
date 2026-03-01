const mockGet = jest.fn();

function loadModule(platform: string, nativeModule: unknown = null) {
  jest.resetModules();
  mockGet.mockReset();
  mockGet.mockReturnValue(nativeModule);

  jest.doMock('react-native', () => ({
    Platform: {
      OS: platform,
    },
    TurboModuleRegistry: {
      get: mockGet,
    },
  }));

  return require('../index');
}

describe('react-native-launcher-windows wrapper', () => {
  it('throws on non-Windows platforms', async () => {
    const launcher = loadModule('ios');

    await expect(
      launcher.launchProcess({
        filePath: 'C:\\Tools\\helper.exe',
      })
    ).rejects.toThrow('only supported on Windows');
  });

  it('throws when the native module is missing on Windows', async () => {
    const launcher = loadModule('windows');

    await expect(
      launcher.launchProcess({
        filePath: 'C:\\Tools\\helper.exe',
      })
    ).rejects.toThrow('native module is not available');
  });

  it('validates launchProcess filePath', async () => {
    const launcher = loadModule('windows', {
      launchProcess: jest.fn(),
    });

    await expect(
      launcher.launchProcess({
        filePath: '   ',
      })
    ).rejects.toThrow('filePath must be a non-empty string');
  });

  it('validates launchProcess arguments', async () => {
    const launcher = loadModule('windows', {
      launchProcess: jest.fn(),
    });

    await expect(
      launcher.launchProcess({
        filePath: 'C:\\Tools\\helper.exe',
        arguments: ['valid', 1 as unknown as string],
      })
    ).rejects.toThrow('arguments must be an array of strings');
  });

  it('validates launchBatchFile filePath', async () => {
    const launcher = loadModule('windows', {
      launchBatchFile: jest.fn(),
    });

    await expect(
      launcher.launchBatchFile({
        filePath: '',
      })
    ).rejects.toThrow('filePath must be a non-empty string');
  });

  it('validates launchUri uri', async () => {
    const launcher = loadModule('windows', {
      launchUri: jest.fn(),
    });

    await expect(
      launcher.launchUri({
        uri: '',
      })
    ).rejects.toThrow('uri must be a non-empty string');
  });

  it('rejects an empty bringWindowToFront selector', async () => {
    const launcher = loadModule('windows', {
      bringWindowToFront: jest.fn(),
    });

    await expect(launcher.bringWindowToFront({})).rejects.toThrow(
      'Window selector must provide processId'
    );
  });

  it('forwards a valid bringWindowToFront selector', async () => {
    const nativeModule = {
      bringWindowToFront: jest.fn().mockResolvedValue({
        success: true,
        processId: 42,
        windowTitle: 'Companion App',
      }),
    };
    const launcher = loadModule('windows', nativeModule);
    const selector = { executableName: 'CompanionApp.exe' };

    await expect(launcher.bringWindowToFront(selector)).resolves.toEqual({
      success: true,
      processId: 42,
      windowTitle: 'Companion App',
    });
    expect(nativeModule.bringWindowToFront).toHaveBeenCalledWith(selector);
  });

  it('returns native promise results unchanged', async () => {
    const nativeModule = {
      launchProcess: jest.fn().mockResolvedValue({
        success: true,
        method: 'process',
        target: 'C:\\Tools\\helper.exe',
        processId: 101,
      }),
    };
    const launcher = loadModule('windows', nativeModule);

    await expect(
      launcher.launchProcess({
        filePath: 'C:\\Tools\\helper.exe',
        arguments: ['--silent'],
      })
    ).resolves.toEqual({
      success: true,
      method: 'process',
      target: 'C:\\Tools\\helper.exe',
      processId: 101,
    });
  });
});
