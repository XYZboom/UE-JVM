#include "jni_string.h"

#include "JVM.h"
#include "JVMManager.h"
#include "UJNI.h"

namespace UJNI {
	void ConvertJavaStringToUTF8(JNIEnv* env, jstring str, std::string* result) {
		ensure(str);
		if (!str) {
			UE_LOG(LogJVM, Warning, TEXT("ConvertJavaStringToUTF8 called with null string."))
			result->clear();
			return;
		}
		const jsize length = env->GetStringLength(str);
		if (length <= 0) {
			result->clear();
			CheckException(env);
			return;
		}
		// JNI's GetStringUTFChars() returns strings in Java "modified" UTF8, so
		// instead get the String in UTF16 and convert using chromium's conversion
		// function that yields plain (non Java-modified) UTF8.
		const char* chars = env->GetStringUTFChars(str, nullptr);
		ensure(chars);
		env->ReleaseStringUTFChars(str, chars);
		CheckException(env);
	}

	std::string ConvertJavaStringToUTF8(JNIEnv* env, jstring str) {
		std::string result;
		ConvertJavaStringToUTF8(env, str, &result);
		return result;
	}

	std::string ConvertJavaStringToUTF8(const JavaRef<jstring>& str) {
		return ConvertJavaStringToUTF8(JVMManager::Instance.AttachCurrentThread(), str.obj());
	}

	std::string ConvertJavaStringToUTF8(JNIEnv* env, const JavaRef<jstring>& str) {
		return ConvertJavaStringToUTF8(env, str.obj());
	}

	void ConvertJavaStringToUTF16(JNIEnv* env,
	                              jstring str,
	                              std::u16string* result) {
		ensure(str);
		if (!str) {
			UE_LOG(LogJVM, Warning, TEXT("ConvertJavaStringToUTF16 called with null string."))
			result->clear();
			return;
		}
		const jsize length = env->GetStringLength(str);
		if (length <= 0) {
			result->clear();
			CheckException(env);
			return;
		}
		const jchar* chars = env->GetStringChars(str, NULL);
		ensure(chars);
		// GetStringChars isn't required to NULL-terminate the strings
		// it returns, so the length must be explicitly checked.
		result->assign(reinterpret_cast<const char16_t*>(chars),
		               static_cast<size_t>(length));
		env->ReleaseStringChars(str, chars);
		CheckException(env);
	}

	std::u16string ConvertJavaStringToUTF16(JNIEnv* env, jstring str) {
		std::u16string result;
		ConvertJavaStringToUTF16(env, str, &result);
		return result;
	}

	std::u16string ConvertJavaStringToUTF16(const JavaRef<jstring>& str) {
		return ConvertJavaStringToUTF16(JVMManager::Instance.AttachCurrentThread(), str.obj());
	}

	std::u16string ConvertJavaStringToUTF16(JNIEnv* env,
	                                        const JavaRef<jstring>& str) {
		return ConvertJavaStringToUTF16(env, str.obj());
	}

	ScopedJavaLocalRef<jstring> ConvertUTF8ToJavaString(JNIEnv* env, char* str) {
		return ScopedJavaLocalRef<jstring>(env, env->NewStringUTF(str));
	}
}
