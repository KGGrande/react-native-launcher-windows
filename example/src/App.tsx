import { useState } from 'react';
import {
  Button,
  SafeAreaView,
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  View,
} from 'react-native';
import {
  bringWindowToFront,
  launchBatchFile,
  launchProcess,
  launchUri,
} from 'react-native-launcher-windows';

function normalizeText(value: string): string | undefined {
  const trimmed = value.trim();
  return trimmed.length > 0 ? trimmed : undefined;
}

function parseArguments(value: string): string[] | undefined {
  const trimmed = value.trim();
  if (!trimmed) {
    return undefined;
  }

  const parts = trimmed.split(/\s+/).filter(Boolean);
  return parts.length > 0 ? parts : undefined;
}

export default function App() {
  const [processPath, setProcessPath] = useState('');
  const [processArguments, setProcessArguments] = useState('');
  const [processWorkingDirectory, setProcessWorkingDirectory] = useState('');
  const [batchPath, setBatchPath] = useState('');
  const [batchArguments, setBatchArguments] = useState('');
  const [batchWorkingDirectory, setBatchWorkingDirectory] = useState('');
  const [uri, setUri] = useState('https://reactnative.dev');
  const [windowProcessId, setWindowProcessId] = useState('');
  const [windowExactTitle, setWindowExactTitle] = useState('');
  const [windowTitleContains, setWindowTitleContains] = useState('');
  const [windowExecutableName, setWindowExecutableName] = useState('');
  const [result, setResult] = useState('Ready.');
  const [error, setError] = useState<string | null>(null);

  async function runAction(action: () => Promise<unknown>) {
    setError(null);

    try {
      const nextResult = await action();
      setResult(JSON.stringify(nextResult, null, 2));
    } catch (actionError) {
      setResult('');
      setError(
        actionError instanceof Error ? actionError.message : String(actionError)
      );
    }
  }

  return (
    <SafeAreaView style={styles.screen}>
      <ScrollView contentContainerStyle={styles.content}>
        <Text style={styles.title}>react-native-launcher-windows</Text>
        <Text style={styles.subtitle}>
          Use this screen to launch local desktop targets and focus existing
          windows.
        </Text>

        <View style={styles.section}>
          <Text style={styles.sectionTitle}>Launch Process</Text>
          <TextInput
            placeholder="C:\\Tools\\Helper.exe"
            placeholderTextColor="#666"
            style={styles.input}
            value={processPath}
            onChangeText={setProcessPath}
          />
          <TextInput
            placeholder="Arguments (space-separated)"
            placeholderTextColor="#666"
            style={styles.input}
            value={processArguments}
            onChangeText={setProcessArguments}
          />
          <TextInput
            placeholder="Working directory (optional)"
            placeholderTextColor="#666"
            style={styles.input}
            value={processWorkingDirectory}
            onChangeText={setProcessWorkingDirectory}
          />
          <Button
            color="blue"
            title="Launch Process"
            onPress={() =>
              void runAction(() =>
                launchProcess({
                  filePath: processPath,
                  arguments: parseArguments(processArguments),
                  workingDirectory: normalizeText(processWorkingDirectory),
                })
              )
            }
          />
        </View>

        <View style={styles.section}>
          <Text style={styles.sectionTitle}>Launch Batch File</Text>
          <TextInput
            placeholder="C:\\Scripts\\helper.bat"
            placeholderTextColor="#666"
            style={styles.input}
            value={batchPath}
            onChangeText={setBatchPath}
          />
          <TextInput
            placeholder="Arguments (space-separated)"
            placeholderTextColor="#666"
            style={styles.input}
            value={batchArguments}
            onChangeText={setBatchArguments}
          />
          <TextInput
            placeholder="Working directory (optional)"
            placeholderTextColor="#666"
            style={styles.input}
            value={batchWorkingDirectory}
            onChangeText={setBatchWorkingDirectory}
          />
          <Button
            color="blue"
            title="Launch Batch File"
            onPress={() =>
              void runAction(() =>
                launchBatchFile({
                  filePath: batchPath,
                  arguments: parseArguments(batchArguments),
                  workingDirectory: normalizeText(batchWorkingDirectory),
                })
              )
            }
          />
        </View>

        <View style={styles.section}>
          <Text style={styles.sectionTitle}>Launch URI</Text>
          <TextInput
            placeholder="https://reactnative.dev"
            placeholderTextColor="#666"
            style={styles.input}
            value={uri}
            onChangeText={setUri}
          />
          <Button
            color="blue"
            title="Launch URI"
            onPress={() =>
              void runAction(() =>
                launchUri({
                  uri,
                })
              )
            }
          />
        </View>

        <View style={styles.section}>
          <Text style={styles.sectionTitle}>Bring Window To Front</Text>
          <TextInput
            placeholder="Process ID (optional)"
            placeholderTextColor="#666"
            keyboardType="number-pad"
            style={styles.input}
            value={windowProcessId}
            onChangeText={setWindowProcessId}
          />
          <TextInput
            placeholder="Exact title (optional)"
            placeholderTextColor="#666"
            style={styles.input}
            value={windowExactTitle}
            onChangeText={setWindowExactTitle}
          />
          <TextInput
            placeholder="Title contains (optional)"
            placeholderTextColor="#666"
            style={styles.input}
            value={windowTitleContains}
            onChangeText={setWindowTitleContains}
          />
          <TextInput
            placeholder="Executable name (optional)"
            placeholderTextColor="#666"
            style={styles.input}
            value={windowExecutableName}
            onChangeText={setWindowExecutableName}
          />
          <Button
            color="blue"
            title="Bring Window To Front"
            onPress={() =>
              void runAction(() =>
                bringWindowToFront({
                  ...(normalizeText(windowProcessId)
                    ? {
                        processId: Number.parseInt(windowProcessId, 10),
                      }
                    : {}),
                  ...(normalizeText(windowExactTitle)
                    ? { exactTitle: normalizeText(windowExactTitle) }
                    : {}),
                  ...(normalizeText(windowTitleContains)
                    ? { titleContains: normalizeText(windowTitleContains) }
                    : {}),
                  ...(normalizeText(windowExecutableName)
                    ? { executableName: normalizeText(windowExecutableName) }
                    : {}),
                })
              )
            }
          />
        </View>

        <View style={styles.output}>
          <Text style={styles.sectionTitle}>Result</Text>
          <Text style={styles.outputText}>{error ?? result}</Text>
        </View>
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  screen: {
    flex: 1,
    backgroundColor: '#f3f4f6',
  },
  content: {
    padding: 16,
    gap: 16,
  },
  title: {
    fontSize: 28,
    fontWeight: '700',
    color: '#111827',
  },
  subtitle: {
    fontSize: 14,
    lineHeight: 20,
    color: '#374151',
  },
  section: {
    padding: 16,
    borderRadius: 12,
    backgroundColor: '#ffffff',
    gap: 12,
  },
  sectionTitle: {
    fontSize: 18,
    fontWeight: '600',
    color: '#111827',
  },
  input: {
    borderWidth: 1,
    borderColor: '#d1d5db',
    borderRadius: 10,
    paddingHorizontal: 12,
    paddingVertical: 10,
    color: '#111827',
    backgroundColor: '#f9fafb',
  },
  output: {
    padding: 16,
    borderRadius: 12,
    backgroundColor: '#111827',
    gap: 12,
  },
  outputText: {
    color: '#f9fafb',
    fontFamily: 'Courier New',
  },
});
