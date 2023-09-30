#pragma once
#include "jni.h"

class JVM_API JVMManager {
public:
	void StartJvm(const char **OptionStrings, int Len);
	static JVMManager Instance;
	JNIEnv* GetEnv() const;
	void* JvmHandle = nullptr;
	JNIEnv* Env = nullptr;
	JavaVM* Jvm = nullptr;
	JNIEnv* AttachCurrentThread() const;
	void InitJVM();
	void ReleaseJVM();
private:
	JVMManager() = default;
};
