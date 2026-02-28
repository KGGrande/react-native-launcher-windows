#include <jni.h>
#include "react-native-launcher-windows.h"

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_launcherwindows_LauncherWindowsModule_nativeMultiply(JNIEnv *env, jclass type, jdouble a, jdouble b) {
    return launcherwindows::multiply(a, b);
}
