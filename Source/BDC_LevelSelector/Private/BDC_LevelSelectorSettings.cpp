/* Copyright © beginning at 2025 - BlackDevilCreations
* Author: Patrick Wenzel
* All rights reserved.
* This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
* or modified without prior written permission from BlackDevilCreations.
* Unreal Engine and its associated trademarks are property of Epic Games, Inc.
* and are used with permission.
*/
#include "BDC_LevelSelectorSettings.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"

void UBDC_LevelSelectorSettings::SaveToProjectDefaultConfig()
{
	TryUpdateDefaultConfigFile();
}

UBDC_LevelSelectorSettings::UBDC_LevelSelectorSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("BDC Level Selector");
}

void UBDC_LevelSelectorSettings::AddFavorite(UWorld* TargetedLevel)
{
	if (TargetedLevel)
	{
		if(const FSoftObjectPath Path(TargetedLevel); !FavoriteLevels.Contains(Path))
		{
			FavoriteLevels.Add(Path);
		}
		SaveToProjectDefaultConfig();
	}
}

void UBDC_LevelSelectorSettings::RemoveFavorite(UWorld* TargetedLevel)
{
	if (TargetedLevel)
	{
		const FSoftObjectPath Path(TargetedLevel);
		FavoriteLevels.Remove(Path);
		SaveToProjectDefaultConfig();
	}
}

void UBDC_LevelSelectorSettings::SetLevelTag(UWorld* TargetedLevel, FGameplayTag NewTag)
{
	if (TargetedLevel)
	{
		const FSoftObjectPath Path(TargetedLevel);
		LevelTags.Add(Path, NewTag);
		SaveToProjectDefaultConfig();
	}
}

FGameplayTag UBDC_LevelSelectorSettings::GetLevelTag(UWorld* TargetedLevel)
{
	if (TargetedLevel)
	{
		const FSoftObjectPath Path(TargetedLevel);
		if (const FGameplayTag* Found = LevelTags.Find(Path))
		{
			return *Found;
		}
	}
	return FGameplayTag();
}
