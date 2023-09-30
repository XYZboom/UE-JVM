// Copyright Epic Games, Inc. All Rights Reserved.

#include "JVM.h"
#include "Core.h"
#include "JVMManager.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
DEFINE_LOG_CATEGORY(LogJVM)

#define LOCTEXT_NAMESPACE "FJVMModule"

void FJVMModule::StartupModule() {
	JVMManager::Instance.InitJVM();
}

void FJVMModule::ShutdownModule() {
	JVMManager::Instance.ReleaseJVM();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJVMModule, JVM)
