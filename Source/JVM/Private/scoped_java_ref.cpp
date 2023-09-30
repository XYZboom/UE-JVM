#include "scoped_java_ref.h"

#include "JVMManager.h"

namespace UJNI {
	namespace {
		constexpr int kDefaultLocalFrameCapacity = 16;
	} // namespace

	ScopedJavaLocalFrame::ScopedJavaLocalFrame(JNIEnv* env) : env_(env) {
		const int failed = env_->PushLocalFrame(kDefaultLocalFrameCapacity);
		ensure(!failed);
	}

	ScopedJavaLocalFrame::ScopedJavaLocalFrame(JNIEnv* env, int capacity)
		: env_(env) {
		const int failed = env_->PushLocalFrame(capacity);
		ensure(!failed);
	}

	ScopedJavaLocalFrame::~ScopedJavaLocalFrame() {
		env_->PopLocalFrame(nullptr);
	}
	
	JNIEnv* JavaRef<jobject>::SetNewLocalRef(JNIEnv* env, jobject obj) {
		if (!env) {
			env = JVMManager::Instance.AttachCurrentThread();
		} else {
			ensure(env == JVMManager::Instance.AttachCurrentThread()); // Is |env| on correct thread.
		}
		if (obj)
			obj = env->NewLocalRef(obj);
		if (obj_)
			env->DeleteLocalRef(obj_);
		obj_ = obj;
		return env;
	}

	void JavaRef<jobject>::SetNewGlobalRef(JNIEnv* env, jobject obj) {
		if (!env) {
			env = JVMManager::Instance.AttachCurrentThread();
		} else {
			ensure(env == JVMManager::Instance.AttachCurrentThread()); // Is |env| on correct thread.
		}
		if (obj)
			obj = env->NewGlobalRef(obj);
		if (obj_)
			env->DeleteGlobalRef(obj_);
		obj_ = obj;
	}

	void JavaRef<jobject>::ResetLocalRef(JNIEnv* env) {
		if (obj_) {
			ensure(env == JVMManager::Instance.AttachCurrentThread()); // Is |env| on correct thread.
			env->DeleteLocalRef(obj_);
			obj_ = nullptr;
		}
	}

	void JavaRef<jobject>::ResetGlobalRef() {
		if (obj_) {
			JVMManager::Instance.AttachCurrentThread()->DeleteGlobalRef(obj_);
			obj_ = nullptr;
		}
	}

	jobject JavaRef<jobject>::ReleaseInternal() {
		jobject obj = obj_;
		obj_ = nullptr;
		return obj;
	}
}
