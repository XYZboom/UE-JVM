#pragma once
#include <atomic>

#include "scoped_java_ref.h"

// Used to export JNI registration functions.
#if defined(COMPONENT_BUILD)
#define JNI_REGISTRATION_EXPORT __attribute__((visibility("default")))
#else
#define JNI_REGISTRATION_EXPORT
#endif

#define CHECK_NATIVE_PTR(env, jcaller, native_ptr, method_name, ...) \
	ensureMsgf(native_ptr, "%hs", method_name);

namespace UJNI {
	// Used to mark symbols to be exported in a shared library's symbol table.
#define JNI_EXPORT __attribute__ ((visibility("default")))

	// Contains the registration method information for initializing JNI bindings.
	struct RegistrationMethod {
		const char* name;
		bool (*func)(JNIEnv* env);
	};
	JVM_API bool HasException(JNIEnv* env);

	// Gets a ClassLoader instance which can load Java classes from the specified
	// split.
	jobject GetSplitClassLoader(JNIEnv* env, const char* split_name);

	// Gets a ClassLoader instance capable of loading Chromium java classes.
	// This should be called either from JNI_OnLoad or from within a method called
	// via JNI from Java.
	inline jobject GetClassLoader(JNIEnv* env) {
		return GetSplitClassLoader(env, "");
	}

	// Initializes the global ClassLoader used by the GetClass and LazyGetClass
	// methods. This is needed because JNI will use the base ClassLoader when there
	// is no Java code on the stack. The base ClassLoader doesn't know about any of
	// the application classes and will fail to lookup anything other than system
	// classes.
	void InitGlobalClassLoader(JNIEnv* env);

	// Finds the class named |class_name| and returns it.
	// Use this method instead of invoking directly the JNI FindClass method (to
	// prevent leaking local references).
	// This method triggers a fatal assertion if the class could not be found.
	// Use HasClass if you need to check whether the class exists.
	JVM_API ScopedJavaLocalRef<jclass> GetClass(JNIEnv* env,
	                                            const char* class_name,
	                                            const char* split_name);
	JVM_API ScopedJavaLocalRef<jclass> GetClass(JNIEnv* env,
	                                            const char* class_name);

	// The method will initialize |atomic_class_id| to contain a global ref to the
	// class. And will return that ref on subsequent calls.  It's the caller's
	// responsibility to release the ref when it is no longer needed.
	// The caller is responsible to zero-initialize |atomic_method_id|.
	// It's fine to simultaneously call this on multiple threads referencing the
	// same |atomic_method_id|.
	JVM_API jclass LazyGetClass(JNIEnv* env,
	                            const char* class_name,
	                            const char* split_name,
	                            std::atomic<jclass>* atomic_class_id);
	JVM_API jclass LazyGetClass(
		JNIEnv* env,
		const char* class_name,
		std::atomic<jclass>* atomic_class_id);

	// This class is a wrapper for JNIEnv Get(Static)MethodID.
	class JVM_API MethodID {
	public:
		enum Type {
			TYPE_STATIC,
			TYPE_INSTANCE,
		};

		// Returns the method ID for the method with the specified name and signature.
		// This method triggers a fatal assertion if the method could not be found.
		template <Type type>
		static jmethodID Get(JNIEnv* env,
		                     jclass clazz,
		                     const char* method_name,
		                     const char* jni_signature);

		// The caller is responsible to zero-initialize |atomic_method_id|.
		// It's fine to simultaneously call this on multiple threads referencing the
		// same |atomic_method_id|.
		template <Type type>
		static jmethodID LazyGet(JNIEnv* env,
		                         jclass clazz,
		                         const char* method_name,
		                         const char* jni_signature,
		                         std::atomic<jmethodID>* atomic_method_id);
	};

	// Returns true if an exception is pending in the provided JNIEnv*.
	JVM_API bool HasException(JNIEnv* env);

	// If an exception is pending in the provided JNIEnv*, this function clears it
	// and returns true.
	JVM_API bool ClearException(JNIEnv* env);

	// This function will call CHECK() macro if there's any pending exception.
	JVM_API void CheckException(JNIEnv* env);
}
