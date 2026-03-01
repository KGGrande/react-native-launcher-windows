#pragma once

#include "pch.h"
#include "resource.h"
#include "codegen/NativeLauncherWindowsDataTypes.g.h"
#include "codegen/NativeLauncherWindowsSpec.g.h"

#include "NativeModules.h"

namespace winrt::ReactNativeLauncherWindows
{
namespace React = winrt::Microsoft::ReactNative;
namespace Codegen = ReactNativeLauncherWindowsCodegen;

REACT_TURBO_MODULE(ReactNativeLauncherWindows, L"LauncherWindows")
struct ReactNativeLauncherWindows
{
  using ModuleSpec = Codegen::LauncherWindowsSpec;
  using LaunchBatchFileOptions = Codegen::LauncherWindowsSpec_LaunchBatchFileOptions;
  using LaunchProcessOptions = Codegen::LauncherWindowsSpec_LaunchProcessOptions;
  using LaunchResult = Codegen::LauncherWindowsSpec_NativeLaunchResult;
  using LaunchUriOptions = Codegen::LauncherWindowsSpec_LaunchUriOptions;
  using BringToFrontResult = Codegen::LauncherWindowsSpec_NativeBringToFrontResult;
  using WindowSelector = Codegen::LauncherWindowsSpec_WindowSelector;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(launchProcess)
  void launchProcess(
      LaunchProcessOptions &&options,
      React::ReactPromise<LaunchResult> &&result) noexcept;

  REACT_METHOD(launchBatchFile)
  void launchBatchFile(
      LaunchBatchFileOptions &&options,
      React::ReactPromise<LaunchResult> &&result) noexcept;

  REACT_METHOD(launchUri)
  void launchUri(LaunchUriOptions &&options, React::ReactPromise<LaunchResult> &&result) noexcept;

  REACT_METHOD(bringWindowToFront)
  void bringWindowToFront(
      WindowSelector &&selector,
      React::ReactPromise<BringToFrontResult> &&result) noexcept;

private:
  React::ReactContext m_context{nullptr};
};

} // namespace winrt::ReactNativeLauncherWindows
