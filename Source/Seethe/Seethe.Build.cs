// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Seethe : ModuleRules
{
	public Seethe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara"
		]);

		PublicIncludePaths.AddRange([
			"Seethe/Interfaces", "Seethe/Inventory", "Seethe/UI", "Seethe/Weapons", "Seethe/Enemies", "Seethe/Tools",
			"Seethe/Interactables"
		]);

		// Uncomment this if you want to visualize line traces
		// PublicDefinitions.Add("SEETHE_ENABLE_DEBUG_TRACES=1");

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}