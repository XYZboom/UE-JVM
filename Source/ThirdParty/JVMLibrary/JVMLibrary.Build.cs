// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class JVMLibrary : ModuleRules {
	public JVMLibrary(ReadOnlyTargetRules Target) : base(Target) {
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64) {
			// Add the import library
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "win64", "include"));
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "win64", "include", "win32"));
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory));
			PublicDelayLoadDLLs.Add("jvm.dll");
		} else if (Target.Platform == UnrealTargetPlatform.Mac) {
			PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
			RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/JVMLibrary/Mac/Release/libExampleLibrary.dylib");
		} else if (Target.Platform == UnrealTargetPlatform.Linux) {
			string ExampleSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "JVMLibrary", "Linux",
				"x86_64-unknown-linux-gnu", "libExampleLibrary.so");
			PublicAdditionalLibraries.Add(ExampleSoPath);
			PublicDelayLoadDLLs.Add(ExampleSoPath);
			RuntimeDependencies.Add(ExampleSoPath);
		}
	}
}