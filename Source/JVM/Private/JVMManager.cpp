#include "JVMManager.h"

#include "Interfaces/IPluginManager.h"

typedef jint (JNICALL *JNICREATEPROC)(JavaVM**, void**, void*);

JVMManager JVMManager::Instance = JVMManager();

JNIEnv* JVMManager::AttachCurrentThread() const {
	ensure(Jvm);
	JNIEnv* env = nullptr;
	jint ret = Jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_2);
	if (ret == JNI_EDETACHED || !env) {
		JavaVMAttachArgs args;
		args.version = JNI_VERSION_1_2;
		args.group = nullptr;
		args.name = nullptr;
		ret = Jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), &args);
		ensure(JNI_OK == ret);
	}
	return env;
}

void JVMManager::InitJVM() {
	UE_LOG(LogLoad, Log, TEXT("Starting Init JVM"));
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("JVM")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/JVMLibrary/Win64/jre/bin/server/jvm.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/JVMLibrary/Mac/Release/libExampleLibrary.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/JVMLibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif // PLATFORM_WINDOWS

	UE_LOG(LogInit, Log, TEXT("JVM DLL path: %s"), *FPaths::ConvertRelativePathToFull(LibraryPath));
	JvmHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (!JvmHandle) {
		UE_LOG(LogInit, Log, TEXT("Init JVM DLL Failed!"))
	} else {
		UE_LOG(LogInit, Log, TEXT("Init JVM DLL Success!"))
	}
}

void JVMManager::ReleaseJVM() {
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(JvmHandle);
	JvmHandle = nullptr;
	if (Jvm) {
		Jvm->DestroyJavaVM();
		Jvm = nullptr;
		Env = nullptr;
	}
}

void JVMManager::StartJvm(const char** OptionStrings, const int Len) {
	JavaVMOption* VMOption;
	int nOptionCount;
	if (Len <= 0) {
		nOptionCount = 2;
		//java虚拟机启动时接收的参数，每个参数单独一项
		VMOption = new JavaVMOption[2];
		//设置JVM最大允许分配的堆内存，按需分配
		VMOption[0].optionString = "-Xmx2048M";
		//设置classpath
		VMOption[1].optionString = "-Djava.class.path=.";
	} else {
		nOptionCount = Len;
		VMOption = new JavaVMOption[Len];
		for (int i = 0; i < Len; ++i) {
			VMOption[i].optionString = OptionStrings[i];
			VMOption[i].extraInfo = nullptr;
		}
	}

	JavaVMInitArgs vmInitArgs;
	vmInitArgs.version = JNI_VERSION_1_8;
	vmInitArgs.options = VMOption;
	vmInitArgs.nOptions = nOptionCount;
	//忽略无法识别jvm的情况
	vmInitArgs.ignoreUnrecognized = JNI_TRUE;

	//初始化jvm物理地址
	const auto jvmProcAddress = static_cast<JNICREATEPROC>(FPlatformProcess::GetDllExport(
		JvmHandle, TEXT("JNI_CreateJavaVM")));

	//创建JVM
	jint jvmProc = (jvmProcAddress)(&Jvm, reinterpret_cast<void**>(&Env), &vmInitArgs);
	if (jvmProc < 0 || Jvm == nullptr || Env == nullptr) {
		FPlatformProcess::FreeDllHandle(JvmHandle);
	}
}

JNIEnv* JVMManager::GetEnv() const {
	return Env;
}
