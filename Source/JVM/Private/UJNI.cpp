#include "UJNI.h"

#include "jni_string.h"
#include "JVM.h"

namespace UJNI {
	namespace {
		JavaVM* g_jvm = nullptr;

		bool g_fatal_exception_occurred = false;

		ScopedJavaLocalRef<jclass> GetClassInternal(JNIEnv* env,
		                                            const char* class_name) {
			const jclass clazz = env->FindClass(class_name);

			if (ClearException(env) || !clazz) {
				UE_LOG(LogJVM, Fatal, TEXT("Failed to find class %hs"), class_name)
			}
			return ScopedJavaLocalRef<jclass>(env, clazz);
		}
	} // namespace

	ScopedJavaLocalRef<jclass> GetClass(JNIEnv* env,
	                                    const char* class_name,
	                                    const char* split_name) {
		return GetClassInternal(env, class_name);
	}

	ScopedJavaLocalRef<jclass> GetClass(JNIEnv* env, const char* class_name) {
		return GetClassInternal(env, class_name);
	}

	// This is duplicated with LazyGetClass below because these are performance
	// sensitive.
	jclass LazyGetClass(JNIEnv* env,
	                    const char* class_name,
	                    const char* split_name,
	                    std::atomic<jclass>* atomic_class_id) {
		const jclass value = atomic_class_id->load(std::memory_order_acquire);
		if (value)
			return value;
		ScopedJavaGlobalRef<jclass> clazz;
		clazz.Reset(GetClass(env, class_name, split_name));
		jclass cas_result = nullptr;
		if (atomic_class_id->compare_exchange_strong(cas_result, clazz.obj(),
		                                             std::memory_order_acq_rel)) {
			// We intentionally leak the global ref since we now storing it as a raw
			// pointer in |atomic_class_id|.
			return clazz.Release();
		}
		return cas_result;
	}

	// This is duplicated with LazyGetClass above because these are performance
	// sensitive.
	jclass LazyGetClass(JNIEnv* env,
	                    const char* class_name,
	                    std::atomic<jclass>* atomic_class_id) {
		const jclass value = atomic_class_id->load(std::memory_order_acquire);
		if (value)
			return value;
		ScopedJavaGlobalRef<jclass> clazz;
		clazz.Reset(GetClass(env, class_name));
		jclass cas_result = nullptr;
		if (atomic_class_id->compare_exchange_strong(cas_result, clazz.obj(),
		                                             std::memory_order_acq_rel)) {
			// We intentionally leak the global ref since we now storing it as a raw
			// pointer in |atomic_class_id|.
			return clazz.Release();
		}
		return cas_result;
	}

	template <MethodID::Type type>
	jmethodID MethodID::Get(JNIEnv* env,
	                        jclass clazz,
	                        const char* method_name,
	                        const char* jni_signature) {
		auto get_method_ptr = type == MethodID::TYPE_STATIC ? &JNIEnv::GetStaticMethodID : &JNIEnv::GetMethodID;
		jmethodID id = (env->*get_method_ptr)(clazz, method_name, jni_signature);
		if (ClearException(env) || !id) {
			UE_LOG(LogJVM, Fatal, TEXT("Failed to find %hs method %hs %hs"),
			       type == TYPE_STATIC ? "static " : "", method_name, jni_signature)
		}
		return id;
	}

	// If |atomic_method_id| set, it'll return immediately. Otherwise, it'll call
	// into ::Get() above. If there's a race, it's ok since the values are the same
	// (and the duplicated effort will happen only once).
	template <MethodID::Type type>
	jmethodID MethodID::LazyGet(JNIEnv* env,
	                            jclass clazz,
	                            const char* method_name,
	                            const char* jni_signature,
	                            std::atomic<jmethodID>* atomic_method_id) {
		const jmethodID value = atomic_method_id->load(std::memory_order_acquire);
		if (value)
			return value;
		jmethodID id = MethodID::Get<type>(env, clazz, method_name, jni_signature);
		atomic_method_id->store(id, std::memory_order_release);
		return id;
	}

	// Various template instantiations.
	template jmethodID MethodID::Get<MethodID::TYPE_STATIC>(
		JNIEnv* env, jclass clazz, const char* method_name,
		const char* jni_signature);

	template jmethodID MethodID::Get<MethodID::TYPE_INSTANCE>(
		JNIEnv* env, jclass clazz, const char* method_name,
		const char* jni_signature);

	template jmethodID MethodID::LazyGet<MethodID::TYPE_STATIC>(
		JNIEnv* env, jclass clazz, const char* method_name,
		const char* jni_signature, std::atomic<jmethodID>* atomic_method_id);

	template jmethodID MethodID::LazyGet<MethodID::TYPE_INSTANCE>(
		JNIEnv* env, jclass clazz, const char* method_name,
		const char* jni_signature, std::atomic<jmethodID>* atomic_method_id);

	bool HasException(JNIEnv* env) {
		return env->ExceptionCheck() != JNI_FALSE;
	}

	bool ClearException(JNIEnv* env) {
		if (!HasException(env))
			return false;
		env->ExceptionDescribe();
		env->ExceptionClear();
		return true;
	}

	void CheckException(JNIEnv* env) {
		if (!HasException(env))
			return;

		const ScopedJavaLocalRef<jthrowable> throwable(env, env->ExceptionOccurred());
		if (throwable) {
			// Clear the pending exception, since a local reference is now held.
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
}
