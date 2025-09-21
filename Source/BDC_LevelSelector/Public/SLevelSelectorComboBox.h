/* Copyright © beginning at 2025 - BlackDevilCreations
  * Author: Patrick Wenzel
  * All rights reserved.
  * 
  * This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
  * or modified without prior written permission from BlackDevilCreations.
  * 
  * Unreal Engine and its associated trademarks are property of Epic Games, Inc.
  * and are used with permission.
*/
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "GameplayTagContainer.h"
#include "Templates/UniquePtr.h"

struct FAssetData;
class SBox;
struct FSlateBrush;
class UWorld;
class UTexture2D;

DECLARE_DELEGATE_OneParam(FOnGameplayTagChanged, const FGameplayTag);

struct FLevelSelectorItem
{
	FString DisplayName;
	FString PackagePath;
	FAssetData AssetData;

	explicit FLevelSelectorItem(const FAssetData& InAssetData);

	static TSharedRef<FLevelSelectorItem> Create(const FAssetData& InAssetData)
	{
		return MakeShareable(new FLevelSelectorItem(InAssetData));
	}

	bool IsFavorite() const;
};

class BDC_LEVELSELECTOR_API SLevelSelectorComboBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelSelectorComboBox) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
	virtual ~SLevelSelectorComboBox() override;

private:
	// Data population and selection
	void PopulateLevelList();
	void RefreshSelection(const FString& MapPath, bool bStrict = true);
	void SortLevelList();
	void OnComboBoxOpening();
	void EnsureSelectedCurrentLevel(bool bStrict);
	void HandleMapOpened(const FString& Filename, bool bAsTemplate);
	void OnAssetRegistryFilesLoaded();

	// Combo/list item generation and interactions
	TSharedRef<SWidget> OnGenerateComboWidget(TSharedPtr<FLevelSelectorItem> InItem);
	void OnSelectionChanged(TSharedPtr<FLevelSelectorItem> InItem, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> CreateLevelItemWidget(const TSharedPtr<FLevelSelectorItem>& InItem);
	TSharedRef<SWidget> CreateSelectedItemWidget(const TSharedPtr<FLevelSelectorItem>& InItem);
	TSharedRef<SWidget> CreateTagSelectionWidget(const TSharedPtr<FLevelSelectorItem>& InItem);
	void OnFavoriteCheckboxChanged(ECheckBoxState NewState, TSharedPtr<FLevelSelectorItem> InItem);
	void OnTagChanged(const TSharedPtr<FLevelSelectorItem>& InItem, FGameplayTag NewTag);

	// Filtering header and handlers
	void ApplyFilters();
	bool IsHeaderItem(const TSharedPtr<FLevelSelectorItem>& InItem) const;
	FGameplayTag GetItemTag(const TSharedPtr<FLevelSelectorItem>& InItem) const;
	void OnSearchTextChanged(const FText& InText);
	void OnSearchTextCommitted(const FText& InText, ETextCommit::Type CommitType);
	void OnFilterTagChanged(FGameplayTag InTag);
	FReply OnClearFilterClicked();

	// Buttons
	FReply OnRefreshButtonClicked();
	FReply OnShowInContentBrowserClicked(const TSharedPtr<FLevelSelectorItem>& InItem) const;

	// Data sources
	TArray<TSharedPtr<FLevelSelectorItem>> AllLevels;
	TArray<TSharedPtr<FLevelSelectorItem>> LevelListSource;
	TSharedPtr<FLevelSelectorItem> HeaderItem;

	// Widgets
	TSharedPtr<SComboBox<TSharedPtr<FLevelSelectorItem>>> LevelComboBox;
	TSharedPtr<SBox> ComboBoxContentContainer;
	TSharedPtr<class SEditableTextBox> SearchTextBoxWidget;
	TSharedPtr<class SGameplayTagCombo> FilterTagComboWidget;

	FText SearchTextFilter;
	FGameplayTag SelectedFilterTag;

	const FSlateBrush* DefaultLevelIcon;
	const FSlateBrush* RefreshIconBrush;

	const FSlateBrush* FavoriteIconBrush;
	const FSlateBrush* UnfavoriteIconBrush;

	UTexture2D* FavoriteIconTextureTrue = nullptr;
	UTexture2D* FavoriteIconTextureFalse = nullptr;

	TUniquePtr<FSlateBrush> FavoriteOwnedBrush;
	TUniquePtr<FSlateBrush> UnfavoriteOwnedBrush;
};