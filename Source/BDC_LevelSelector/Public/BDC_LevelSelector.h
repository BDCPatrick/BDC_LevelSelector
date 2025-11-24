/* Copyright © beginning at 2025 - BlackDevilCreations
  * Author: Patrick Wenzel
  * All rights reserved.
  * * This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
  * or modified without prior written permission from BlackDevilCreations.
  * * Unreal Engine and its associated trademarks are property of Epic Games, Inc.
  * and are used with permission.
*/
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SLevelSelectorComboBox;
class SLevelSelectorCameraOverlay;

class BDC_LEVELSELECTOR_API FBDC_LevelSelectorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	// Toolbar
	void AddToolbarExtension(FToolBarBuilder& Builder);
	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<SLevelSelectorComboBox> LevelSelectorWidget;

	// Camera Favorite
	void OnMapOpened(const FString& Filename, bool bAsTemplate);
	void OnPostEngineInit();
	void RefreshOverlay();
	TSharedPtr<SLevelSelectorCameraOverlay> OverlayWidget;
};