// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
JVM_API DECLARE_LOG_CATEGORY_EXTERN(LogJVM, Log, All)

class JVM_API FJVMModule : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
