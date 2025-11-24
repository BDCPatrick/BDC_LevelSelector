/* Copyright © beginning at 2025 - BlackDevilCreations
* Author: Patrick Wenzel
* All rights reserved.
* This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
* or modified without prior written permission from BlackDevilCreations.
* Unreal Engine and its associated trademarks are property of Epic Games, Inc.
* and are used with permission.
*/
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "BDC_LevelSelectorSettings.generated.h"

USTRUCT()
struct FCameraFavorite
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Camera Favorites")
	TMap<FName, FTransform> HoldFavorites;
};

UCLASS(Config=Editor, DefaultConfig)
class BDC_LEVELSELECTOR_API UBDC_LevelSelectorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UBDC_LevelSelectorSettings();

	/** A set of favorite levels. These levels will always appear at the top of the list. */
	UPROPERTY(Config, EditAnywhere, Category = "Level Selector")
	TSet<FSoftObjectPath>FavoriteLevels;
	
	/** A set of favorite levels. These levels will always appear at the top of the list. */
	UPROPERTY(Config, EditAnywhere, Category = "Level Selector")
	TMap<FSoftObjectPath, FGameplayTag> LevelTags;

	/** Adds a level to the favorite levels set. */
	void AddFavorite(UWorld* TargetedLevel);

	/** Removes a level from the favorite levels set. */
	void RemoveFavorite(UWorld* TargetedLevel);
	
	/** Sets a Level Tag. */
	void SetLevelTag(UWorld* TargetedLevel, FGameplayTag NewTag);
	
	/** Returns a Level's Tag. */
	FGameplayTag GetLevelTag(UWorld* TargetedLevel);
	
	/** Holds the Camera favorites per Level.*/
	UPROPERTY(Config, EditAnywhere, Category = "Camera Favorites")
	TMap<TSoftObjectPtr<UWorld>, FCameraFavorite> HoldFavorites;
	
	void SaveToProjectDefaultConfig();
};