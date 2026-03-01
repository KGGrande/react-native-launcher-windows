
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <string>
#include <optional>
#include <functional>
#include <vector>

namespace ReactNativeLauncherWindowsCodegen {

struct LauncherWindowsSpec_LaunchBatchFileOptions {
    std::string filePath;
    std::optional<std::vector<std::string>> arguments;
    std::optional<std::string> workingDirectory;
};

struct LauncherWindowsSpec_LaunchProcessOptions {
    std::string filePath;
    std::optional<std::vector<std::string>> arguments;
    std::optional<std::string> workingDirectory;
};

struct LauncherWindowsSpec_LaunchUriOptions {
    std::string uri;
};

struct LauncherWindowsSpec_NativeBringToFrontResult {
    bool success;
    std::optional<double> processId;
    std::optional<std::string> windowTitle;
};

struct LauncherWindowsSpec_NativeLaunchResult {
    bool success;
    std::string method;
    std::string target;
    std::optional<double> processId;
};

struct LauncherWindowsSpec_WindowSelector {
    std::optional<double> processId;
    std::optional<std::string> exactTitle;
    std::optional<std::string> titleContains;
    std::optional<std::string> executableName;
};

} // namespace ReactNativeLauncherWindowsCodegen
