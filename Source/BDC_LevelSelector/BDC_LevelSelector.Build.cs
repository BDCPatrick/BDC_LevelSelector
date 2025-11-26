/* Copyright © beginning at 2025 - BlackDevilCreations
* Author: Patrick Wenzel
* All rights reserved.
* This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
* or modified without prior written permission from BlackDevilCreations.
* Unreal Engine and its associated trademarks are property of Epic Games, Inc.
* and are used with permission.
*/
using UnrealBuildTool;

public class BDC_LevelSelector : ModuleRules
{
	public BDC_LevelSelector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore", "GameplayTags"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject", 
				"Engine",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"EditorWidgets",
				"UnrealEd",
				"PropertyEditor",
				"GameplayTags",
				"GameplayTagsEditor",
				"ContentBrowser",
				"AssetRegistry",
				"DeveloperSettings",
				"LevelEditor"
			}
		);
	}

}
