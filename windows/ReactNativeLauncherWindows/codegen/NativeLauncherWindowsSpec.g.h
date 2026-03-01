
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

// #include "NativeLauncherWindowsDataTypes.g.h" before this file to use the generated type definition
#include <NativeModules.h>
#include <tuple>

namespace ReactNativeLauncherWindowsCodegen {

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(LauncherWindowsSpec_LaunchBatchFileOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"filePath", &LauncherWindowsSpec_LaunchBatchFileOptions::filePath},
        {L"arguments", &LauncherWindowsSpec_LaunchBatchFileOptions::arguments},
        {L"workingDirectory", &LauncherWindowsSpec_LaunchBatchFileOptions::workingDirectory},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(LauncherWindowsSpec_LaunchProcessOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"filePath", &LauncherWindowsSpec_LaunchProcessOptions::filePath},
        {L"arguments", &LauncherWindowsSpec_LaunchProcessOptions::arguments},
        {L"workingDirectory", &LauncherWindowsSpec_LaunchProcessOptions::workingDirectory},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(LauncherWindowsSpec_LaunchUriOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"uri", &LauncherWindowsSpec_LaunchUriOptions::uri},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(LauncherWindowsSpec_NativeBringToFrontResult*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"success", &LauncherWindowsSpec_NativeBringToFrontResult::success},
        {L"processId", &LauncherWindowsSpec_NativeBringToFrontResult::processId},
        {L"windowTitle", &LauncherWindowsSpec_NativeBringToFrontResult::windowTitle},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(LauncherWindowsSpec_NativeLaunchResult*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"success", &LauncherWindowsSpec_NativeLaunchResult::success},
        {L"method", &LauncherWindowsSpec_NativeLaunchResult::method},
        {L"target", &LauncherWindowsSpec_NativeLaunchResult::target},
        {L"processId", &LauncherWindowsSpec_NativeLaunchResult::processId},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(LauncherWindowsSpec_WindowSelector*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"processId", &LauncherWindowsSpec_WindowSelector::processId},
        {L"exactTitle", &LauncherWindowsSpec_WindowSelector::exactTitle},
        {L"titleContains", &LauncherWindowsSpec_WindowSelector::titleContains},
        {L"executableName", &LauncherWindowsSpec_WindowSelector::executableName},
    };
    return fieldMap;
}

struct LauncherWindowsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(LauncherWindowsSpec_LaunchProcessOptions, Promise<LauncherWindowsSpec_NativeLaunchResult>) noexcept>{0, L"launchProcess"},
      Method<void(LauncherWindowsSpec_LaunchBatchFileOptions, Promise<LauncherWindowsSpec_NativeLaunchResult>) noexcept>{1, L"launchBatchFile"},
      Method<void(LauncherWindowsSpec_LaunchUriOptions, Promise<LauncherWindowsSpec_NativeLaunchResult>) noexcept>{2, L"launchUri"},
      Method<void(LauncherWindowsSpec_WindowSelector, Promise<LauncherWindowsSpec_NativeBringToFrontResult>) noexcept>{3, L"bringWindowToFront"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, LauncherWindowsSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "launchProcess",
          "    REACT_METHOD(launchProcess) void launchProcess(LauncherWindowsSpec_LaunchProcessOptions && options, ::React::ReactPromise<LauncherWindowsSpec_NativeLaunchResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(launchProcess) static void launchProcess(LauncherWindowsSpec_LaunchProcessOptions && options, ::React::ReactPromise<LauncherWindowsSpec_NativeLaunchResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "launchBatchFile",
          "    REACT_METHOD(launchBatchFile) void launchBatchFile(LauncherWindowsSpec_LaunchBatchFileOptions && options, ::React::ReactPromise<LauncherWindowsSpec_NativeLaunchResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(launchBatchFile) static void launchBatchFile(LauncherWindowsSpec_LaunchBatchFileOptions && options, ::React::ReactPromise<LauncherWindowsSpec_NativeLaunchResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "launchUri",
          "    REACT_METHOD(launchUri) void launchUri(LauncherWindowsSpec_LaunchUriOptions && options, ::React::ReactPromise<LauncherWindowsSpec_NativeLaunchResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(launchUri) static void launchUri(LauncherWindowsSpec_LaunchUriOptions && options, ::React::ReactPromise<LauncherWindowsSpec_NativeLaunchResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "bringWindowToFront",
          "    REACT_METHOD(bringWindowToFront) void bringWindowToFront(LauncherWindowsSpec_WindowSelector && selector, ::React::ReactPromise<LauncherWindowsSpec_NativeBringToFrontResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(bringWindowToFront) static void bringWindowToFront(LauncherWindowsSpec_WindowSelector && selector, ::React::ReactPromise<LauncherWindowsSpec_NativeBringToFrontResult> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace ReactNativeLauncherWindowsCodegen
