#pragma once
#include <string>

#include "jni.h"
#include "scoped_java_ref.h"

namespace UJNI {
	// Convert a Java string to UTF8. Returns a std string.
	JVM_API void ConvertJavaStringToUTF8(JNIEnv* env,
	                                     jstring str,
	                                     std::string* result);
	JVM_API std::string ConvertJavaStringToUTF8(JNIEnv* env, jstring str);
	JVM_API std::string ConvertJavaStringToUTF8(const JavaRef<jstring>& str);
	JVM_API std::string ConvertJavaStringToUTF8(JNIEnv* env,
	                                            const JavaRef<jstring>& str);

	// Convert a Java string to UTF16. Returns a std::u16string.
	JVM_API void ConvertJavaStringToUTF16(JNIEnv* env,
	                                      jstring str,
	                                      std::u16string* result);
	JVM_API std::u16string ConvertJavaStringToUTF16(JNIEnv* env, jstring str);
	JVM_API std::u16string ConvertJavaStringToUTF16(
		const JavaRef<jstring>& str);
	JVM_API std::u16string ConvertJavaStringToUTF16(
		JNIEnv* env,
		const JavaRef<jstring>& str);
	JVM_API ScopedJavaLocalRef<jstring> ConvertUTF8ToJavaString(JNIEnv* env, char* str);
}
