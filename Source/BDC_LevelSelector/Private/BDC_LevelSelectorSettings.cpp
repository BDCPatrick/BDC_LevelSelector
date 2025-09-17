#include "BDC_LevelSelectorSettings.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"

void UBDC_LevelSelectorSettings::SaveToProjectDefaultConfig()
{
	const FString DefaultConfigFile = GetDefault<UBDC_LevelSelectorSettings>()->GetDefaultConfigFilename();
	SaveConfig(CPF_Config, *DefaultConfigFile);
	GConfig->Flush(false, *DefaultConfigFile);
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