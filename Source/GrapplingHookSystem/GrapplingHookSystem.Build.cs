// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GrapplingHookSystem : ModuleRules
{
	public GrapplingHookSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
