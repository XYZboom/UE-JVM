// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class JVM : ModuleRules {
	public JVM(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		var files = Directory.GetFiles(Path.Combine(PluginDirectory, "Binaries/ThirdParty/JVMLibrary/Win64")
			, "*", SearchOption.AllDirectories);
		foreach (var file in files) {
			RuntimeDependencies.Add(file);
		}
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"JVMLibrary",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[] {
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}