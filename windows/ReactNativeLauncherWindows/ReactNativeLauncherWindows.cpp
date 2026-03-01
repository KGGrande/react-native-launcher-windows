#include "pch.h"

#include "ReactNativeLauncherWindows.h"

#include <shellapi.h>

#include <algorithm>
#include <cwctype>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace winrt::ReactNativeLauncherWindows
{
namespace React = winrt::Microsoft::ReactNative;

namespace
{
using LaunchBatchFileOptions = ReactNativeLauncherWindows::LaunchBatchFileOptions;
using LaunchProcessOptions = ReactNativeLauncherWindows::LaunchProcessOptions;
using LaunchResult = ReactNativeLauncherWindows::LaunchResult;
using LaunchUriOptions = ReactNativeLauncherWindows::LaunchUriOptions;
using BringToFrontResult = ReactNativeLauncherWindows::BringToFrontResult;
using WindowSelector = ReactNativeLauncherWindows::WindowSelector;

struct unique_handle {
  unique_handle() noexcept = default;
  explicit unique_handle(HANDLE value) noexcept : handle(value) {}
  ~unique_handle() noexcept {
    if (handle != nullptr) {
      ::CloseHandle(handle);
    }
  }

  unique_handle(unique_handle const &) = delete;
  unique_handle &operator=(unique_handle const &) = delete;

  unique_handle(unique_handle &&other) noexcept : handle(other.handle) {
    other.handle = nullptr;
  }

  unique_handle &operator=(unique_handle &&other) noexcept {
    if (this != &other) {
      if (handle != nullptr) {
        ::CloseHandle(handle);
      }

      handle = other.handle;
      other.handle = nullptr;
    }

    return *this;
  }

  HANDLE get() const noexcept {
    return handle;
  }

 private:
  HANDLE handle{nullptr};
};

std::wstring ToWide(std::string const &value) noexcept {
  if (value.empty()) {
    return {};
  }

  const auto requiredSize = ::MultiByteToWideChar(
      CP_UTF8,
      MB_ERR_INVALID_CHARS,
      value.c_str(),
      static_cast<int>(value.size()),
      nullptr,
      0);

  if (requiredSize <= 0) {
    return std::wstring(value.begin(), value.end());
  }

  std::wstring result(static_cast<size_t>(requiredSize), L'\0');
  ::MultiByteToWideChar(
      CP_UTF8,
      MB_ERR_INVALID_CHARS,
      value.c_str(),
      static_cast<int>(value.size()),
      result.data(),
      requiredSize);
  return result;
}

std::string ToUtf8(std::wstring const &value) noexcept {
  if (value.empty()) {
    return {};
  }

  const auto requiredSize = ::WideCharToMultiByte(
      CP_UTF8,
      0,
      value.c_str(),
      static_cast<int>(value.size()),
      nullptr,
      0,
      nullptr,
      nullptr);

  if (requiredSize <= 0) {
    return std::string(value.begin(), value.end());
  }

  std::string result(static_cast<size_t>(requiredSize), '\0');
  ::WideCharToMultiByte(
      CP_UTF8,
      0,
      value.c_str(),
      static_cast<int>(value.size()),
      result.data(),
      requiredSize,
      nullptr,
      nullptr);
  return result;
}

std::optional<std::wstring> ToWideOptional(std::optional<std::string> const &value) noexcept {
  if (!value.has_value()) {
    return std::nullopt;
  }

  return ToWide(*value);
}

std::wstring TrimSystemMessage(std::wstring message) noexcept {
  while (!message.empty()) {
    const auto tail = message.back();
    if (tail != L'\r' && tail != L'\n' && tail != L' ') {
      break;
    }

    message.pop_back();
  }

  return message;
}

std::wstring FormatSystemMessage(DWORD errorCode) noexcept {
  LPWSTR buffer = nullptr;
  const auto length = ::FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,
      errorCode,
      0,
      reinterpret_cast<LPWSTR>(&buffer),
      0,
      nullptr);

  if (length == 0 || buffer == nullptr) {
    return L"Unknown error";
  }

  std::wstring message(buffer, length);
  ::LocalFree(buffer);
  return TrimSystemMessage(std::move(message));
}

React::ReactError MakeReactError(std::string const &code, std::string const &message) noexcept {
  React::ReactError error;
  error.Code = code;
  error.Message = message;
  return error;
}

std::string BuildErrorMessage(
    std::string_view prefix,
    DWORD errorCode,
    std::string const &target) noexcept {
  std::string message(prefix);

  if (!target.empty()) {
    message += " (";
    message += target;
    message += ")";
  }

  message += ". Win32 error ";
  message += std::to_string(errorCode);
  message += ": ";
  message += ToUtf8(FormatSystemMessage(errorCode));
  return message;
}

void RejectWithWin32Error(
    React::ReactPromiseBase const &result,
    std::string const &code,
    std::string_view prefix,
    std::string const &target) noexcept {
  const auto errorCode = ::GetLastError();
  result.Reject(MakeReactError(code, BuildErrorMessage(prefix, errorCode, target)));
}

std::wstring QuoteForCommandLine(std::wstring const &value) noexcept {
  if (value.empty()) {
    return L"\"\"";
  }

  if (value.find_first_of(L" \t\"") == std::wstring::npos) {
    return value;
  }

  std::wstring quoted;
  quoted.push_back(L'"');
  size_t slashCount = 0;

  for (const auto ch : value) {
    if (ch == L'\\') {
      ++slashCount;
      continue;
    }

    if (ch == L'"') {
      quoted.append((slashCount * 2) + 1, L'\\');
      quoted.push_back(L'"');
      slashCount = 0;
      continue;
    }

    if (slashCount > 0) {
      quoted.append(slashCount, L'\\');
      slashCount = 0;
    }

    quoted.push_back(ch);
  }

  if (slashCount > 0) {
    quoted.append(slashCount * 2, L'\\');
  }

  quoted.push_back(L'"');
  return quoted;
}

void AppendArguments(
    std::wstring &commandLine,
    std::optional<std::vector<std::string>> const &arguments) noexcept {
  if (!arguments.has_value()) {
    return;
  }

  for (const auto &argument : *arguments) {
    commandLine.push_back(L' ');
    commandLine += QuoteForCommandLine(ToWide(argument));
  }
}

std::wstring BuildProcessCommandLine(LaunchProcessOptions const &options) noexcept {
  auto commandLine = QuoteForCommandLine(ToWide(options.filePath));
  AppendArguments(commandLine, options.arguments);
  return commandLine;
}

std::wstring ResolveComSpec() noexcept {
  const auto requiredSize = ::GetEnvironmentVariableW(L"ComSpec", nullptr, 0);
  if (requiredSize == 0) {
    return L"C:\\Windows\\System32\\cmd.exe";
  }

  std::wstring value(requiredSize, L'\0');
  const auto written = ::GetEnvironmentVariableW(L"ComSpec", value.data(), requiredSize);
  if (written == 0) {
    return L"C:\\Windows\\System32\\cmd.exe";
  }

  value.resize(written);
  return value;
}

bool LaunchCommand(
    wchar_t const *applicationName,
    std::wstring &commandLine,
    std::optional<std::wstring> const &workingDirectory,
    DWORD &processId) noexcept {
  STARTUPINFOW startupInfo{};
  startupInfo.cb = sizeof(startupInfo);

  PROCESS_INFORMATION processInfo{};

  const auto launched = ::CreateProcessW(
      applicationName,
      commandLine.empty() ? nullptr : commandLine.data(),
      nullptr,
      nullptr,
      FALSE,
      0,
      nullptr,
      workingDirectory.has_value() ? workingDirectory->c_str() : nullptr,
      &startupInfo,
      &processInfo);

  if (launched == FALSE) {
    return false;
  }

  processId = processInfo.dwProcessId;
  ::CloseHandle(processInfo.hThread);
  ::CloseHandle(processInfo.hProcess);
  return true;
}

std::wstring GetWindowTitle(HWND hwnd) noexcept {
  const auto length = ::GetWindowTextLengthW(hwnd);
  if (length <= 0) {
    return {};
  }

  std::wstring title(static_cast<size_t>(length) + 1, L'\0');
  const auto copied = ::GetWindowTextW(hwnd, title.data(), length + 1);
  if (copied <= 0) {
    return {};
  }

  title.resize(copied);
  return title;
}

std::wstring ToLower(std::wstring value) noexcept {
  std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
    return static_cast<wchar_t>(::towlower(ch));
  });
  return value;
}

std::wstring GetBasename(std::wstring const &path) noexcept {
  const auto separatorIndex = path.find_last_of(L"\\/");
  if (separatorIndex == std::wstring::npos) {
    return path;
  }

  return path.substr(separatorIndex + 1);
}

std::optional<std::wstring> GetProcessImagePath(DWORD processId) noexcept {
  unique_handle processHandle(
      ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId));
  if (processHandle.get() == nullptr) {
    return std::nullopt;
  }

  std::wstring buffer(32768, L'\0');
  DWORD size = static_cast<DWORD>(buffer.size());
  if (::QueryFullProcessImageNameW(processHandle.get(), 0, buffer.data(), &size) == FALSE) {
    return std::nullopt;
  }

  buffer.resize(size);
  return buffer;
}

enum class SelectorMode {
  None,
  ProcessId,
  ExactTitle,
  TitleContains,
  ExecutableName,
};

SelectorMode ResolveSelectorMode(WindowSelector const &selector) noexcept {
  if (selector.processId.has_value()) {
    return SelectorMode::ProcessId;
  }

  if (selector.exactTitle.has_value()) {
    return SelectorMode::ExactTitle;
  }

  if (selector.titleContains.has_value()) {
    return SelectorMode::TitleContains;
  }

  if (selector.executableName.has_value()) {
    return SelectorMode::ExecutableName;
  }

  return SelectorMode::None;
}

struct WindowSearchState {
  SelectorMode mode{SelectorMode::None};
  WindowSelector const *selector{nullptr};
  HWND matchedWindow{nullptr};
  DWORD matchedProcessId{0};
  std::wstring matchedTitle;
};

bool MatchesWindow(HWND hwnd, WindowSearchState &state) noexcept {
  if (::IsWindowVisible(hwnd) == FALSE || ::GetWindow(hwnd, GW_OWNER) != nullptr) {
    return false;
  }

  DWORD processId = 0;
  ::GetWindowThreadProcessId(hwnd, &processId);
  if (processId == 0) {
    return false;
  }

  const auto title = GetWindowTitle(hwnd);

  switch (state.mode) {
    case SelectorMode::ProcessId:
      if (processId == static_cast<DWORD>(state.selector->processId.value())) {
        state.matchedWindow = hwnd;
        state.matchedProcessId = processId;
        state.matchedTitle = title;
        return true;
      }
      return false;
    case SelectorMode::ExactTitle:
      if (title == ToWide(state.selector->exactTitle.value())) {
        state.matchedWindow = hwnd;
        state.matchedProcessId = processId;
        state.matchedTitle = title;
        return true;
      }
      return false;
    case SelectorMode::TitleContains: {
      const auto needle = ToWide(state.selector->titleContains.value());
      if (!needle.empty() && title.find(needle) != std::wstring::npos) {
        state.matchedWindow = hwnd;
        state.matchedProcessId = processId;
        state.matchedTitle = title;
        return true;
      }
      return false;
    }
    case SelectorMode::ExecutableName: {
      const auto imagePath = GetProcessImagePath(processId);
      if (!imagePath.has_value()) {
        return false;
      }

      if (ToLower(GetBasename(*imagePath)) ==
          ToLower(ToWide(state.selector->executableName.value()))) {
        state.matchedWindow = hwnd;
        state.matchedProcessId = processId;
        state.matchedTitle = title;
        return true;
      }
      return false;
    }
    case SelectorMode::None:
    default:
      return false;
  }
}

BOOL CALLBACK FindMatchingWindow(HWND hwnd, LPARAM lParam) noexcept {
  auto &state = *reinterpret_cast<WindowSearchState *>(lParam);
  return MatchesWindow(hwnd, state) ? FALSE : TRUE;
}

bool ActivateWindow(HWND hwnd) noexcept {
  if (::IsIconic(hwnd) != FALSE) {
    ::ShowWindow(hwnd, SW_RESTORE);
  } else {
    ::ShowWindow(hwnd, SW_SHOW);
  }

  const auto broughtToTop = ::BringWindowToTop(hwnd) != FALSE;
  const auto setForeground = ::SetForegroundWindow(hwnd) != FALSE;
  return broughtToTop || setForeground;
}
} // namespace

void ReactNativeLauncherWindows::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void ReactNativeLauncherWindows::launchProcess(
    LaunchProcessOptions &&options,
    React::ReactPromise<LaunchResult> &&result) noexcept {
  auto commandLine = BuildProcessCommandLine(options);
  const auto workingDirectory = ToWideOptional(options.workingDirectory);
  const auto applicationName = ToWide(options.filePath);

  DWORD processId = 0;
  if (!LaunchCommand(applicationName.c_str(), commandLine, workingDirectory, processId)) {
    RejectWithWin32Error(
        result,
        "ERR_LAUNCH_PROCESS_FAILED",
        "Failed to launch process",
        options.filePath);
    return;
  }

  result.Resolve(LaunchResult{
      true,
      "process",
      options.filePath,
      static_cast<double>(processId),
  });
}

void ReactNativeLauncherWindows::launchBatchFile(
    LaunchBatchFileOptions &&options,
    React::ReactPromise<LaunchResult> &&result) noexcept {
  const auto comSpec = ResolveComSpec();
  auto batchCommand = QuoteForCommandLine(ToWide(options.filePath));
  AppendArguments(batchCommand, options.arguments);

  auto commandLine = QuoteForCommandLine(comSpec);
  commandLine += L" /d /s /c \"";
  commandLine += batchCommand;
  commandLine += L"\"";

  const auto workingDirectory = ToWideOptional(options.workingDirectory);

  DWORD processId = 0;
  if (!LaunchCommand(comSpec.c_str(), commandLine, workingDirectory, processId)) {
    RejectWithWin32Error(
        result,
        "ERR_LAUNCH_BATCH_FAILED",
        "Failed to launch batch file",
        options.filePath);
    return;
  }

  result.Resolve(LaunchResult{
      true,
      "batch",
      options.filePath,
      static_cast<double>(processId),
  });
}

void ReactNativeLauncherWindows::launchUri(
    LaunchUriOptions &&options,
    React::ReactPromise<LaunchResult> &&result) noexcept {
  const auto uri = ToWide(options.uri);

  SHELLEXECUTEINFOW shellExecuteInfo{};
  shellExecuteInfo.cbSize = sizeof(shellExecuteInfo);
  shellExecuteInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
  shellExecuteInfo.hwnd = nullptr;
  shellExecuteInfo.lpVerb = L"open";
  shellExecuteInfo.lpFile = uri.c_str();
  shellExecuteInfo.nShow = SW_SHOWNORMAL;

  if (::ShellExecuteExW(&shellExecuteInfo) == FALSE) {
    RejectWithWin32Error(
        result,
        "ERR_LAUNCH_URI_FAILED",
        "Failed to launch URI",
        options.uri);
    return;
  }

  if (shellExecuteInfo.hProcess != nullptr) {
    ::CloseHandle(shellExecuteInfo.hProcess);
  }

  result.Resolve(LaunchResult{
      true,
      "uri",
      options.uri,
      std::nullopt,
  });
}

void ReactNativeLauncherWindows::bringWindowToFront(
    WindowSelector &&selector,
    React::ReactPromise<BringToFrontResult> &&result) noexcept {
  WindowSearchState state;
  state.mode = ResolveSelectorMode(selector);
  state.selector = &selector;

  if (state.mode == SelectorMode::None) {
    result.Resolve(BringToFrontResult{
        false,
        std::nullopt,
        std::nullopt,
    });
    return;
  }

  ::SetLastError(ERROR_SUCCESS);
  const auto enumerated =
      ::EnumWindows(&FindMatchingWindow, reinterpret_cast<LPARAM>(&state));
  if (enumerated == FALSE && state.matchedWindow == nullptr) {
    const auto errorCode = ::GetLastError();
    if (errorCode != ERROR_SUCCESS) {
      result.Reject(MakeReactError(
          "ERR_BRING_TO_FRONT_FAILED",
          BuildErrorMessage("Failed while enumerating windows", errorCode, "")));
      return;
    }
  }

  if (state.matchedWindow == nullptr) {
    result.Resolve(BringToFrontResult{
        false,
        std::nullopt,
        std::nullopt,
    });
    return;
  }

  const auto activated = ActivateWindow(state.matchedWindow);

  result.Resolve(BringToFrontResult{
      activated,
      static_cast<double>(state.matchedProcessId),
      state.matchedTitle.empty()
          ? std::nullopt
          : std::optional<std::string>{ToUtf8(state.matchedTitle)},
  });
}

} // namespace winrt::ReactNativeLauncherWindows
