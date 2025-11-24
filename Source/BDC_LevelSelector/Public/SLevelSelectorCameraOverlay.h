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
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboButton.h"

class SEditableTextBox;
struct FSlateRoundedBoxBrush;
class UBDC_LevelSelectorSettings;

class SLevelSelectorCameraOverlay : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelSelectorCameraOverlay) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	bool IsFavListEnabled() const;

private:
	UBDC_LevelSelectorSettings* GetSettings() const;
	void GetCurrentLevelSoftPtr(TSoftObjectPtr<UWorld>& OutLevelPtr) const;
	
	FReply OnAddClicked();
	TSharedRef<SWidget> OnGetMenuContent();
	FReply OnTeleportClicked(FName Key, FTransform Transform);
	FReply OnEditItemClicked(FName Key);
	FReply OnDeleteItemClicked(FName Key);

	FReply OnToggleCollapse();
	void GetCollapseIcon(const FSlateBrush*& OutBrush) const;
	void GetContentVisibility(EVisibility& OutVisibility) const;

	TSharedPtr<SEditableTextBox> NameInputBox;
	TSharedPtr<SComboButton> FavComboButton;
	bool bIsCollapsed;
	TSharedPtr<FSlateRoundedBoxBrush> BackgroundBrush;
};