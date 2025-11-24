/* Copyright © beginning at 2025 - BlackDevilCreations
  * Author: Patrick Wenzel
  * All rights reserved.
  * * This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
  * or modified without prior written permission from BlackDevilCreations.
  * * Unreal Engine and its associated trademarks are property of Epic Games, Inc.
  * and are used with permission.
*/
#include "BDC_LevelSelector.h"
#include "SLevelSelectorComboBox.h"
#include "SLevelSelectorCameraOverlay.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Editor.h"
#include "Misc/CoreDelegates.h"

DEFINE_LOG_CATEGORY_STATIC(LogBDCLevelSelector, All, All);

#define LOCTEXT_NAMESPACE "FBDC_LevelSelectorModule"

#pragma region Module Lifecycle
void FBDC_LevelSelectorModule::StartupModule()
{
	UE_LOG(LogBDCLevelSelector, Warning, TEXT("BDC_LevelSelectorModule::StartupModule() is called."));

	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Play",EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw(this, &FBDC_LevelSelectorModule::AddToolbarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		FEditorDelegates::OnMapOpened.AddRaw(this, &FBDC_LevelSelectorModule::OnMapOpened);
		FCoreDelegates::OnPostEngineInit.AddRaw(this, &FBDC_LevelSelectorModule::OnPostEngineInit);
	}
}

void FBDC_LevelSelectorModule::ShutdownModule()
{
	if (ToolbarExtender.IsValid() && FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolbarExtender);
	}
	FEditorDelegates::OnMapOpened.RemoveAll(this);
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);

	if (OverlayWidget.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
		{
			FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
			const auto ActiveViewportInterface = LevelEditorModule.GetFirstActiveLevelViewport();
			TSharedPtr<SLevelViewport> ActiveViewport = StaticCastSharedPtr<SLevelViewport>(ActiveViewportInterface);

			if (ActiveViewport.IsValid())
			{
				ActiveViewport->RemoveOverlayWidget(OverlayWidget.ToSharedRef());
			}
		}
	}
	OverlayWidget.Reset();
}
#pragma endregion

#pragma region Toolbar Extension
void FBDC_LevelSelectorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	UE_LOG(LogBDCLevelSelector, Warning, TEXT("BDC_LevelSelectorModule::AddToolbarExtension() is called."));

	Builder.AddWidget(
		SAssignNew(LevelSelectorWidget, SLevelSelectorComboBox)
	);
}
#pragma endregion

#pragma region Camera Overlay Logic
void FBDC_LevelSelectorModule::OnPostEngineInit()
{
	if (GEditor)
	{
		GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateRaw(this, &FBDC_LevelSelectorModule::RefreshOverlay));
	}
}

void FBDC_LevelSelectorModule::RefreshOverlay()
{
	if (!FModuleManager::Get().IsModuleLoaded("LevelEditor")) return;

	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	
	if (!OverlayWidget.IsValid())
	{
		OverlayWidget = SNew(SLevelSelectorCameraOverlay);
	}

	auto ActiveViewportInterface = LevelEditorModule.GetFirstActiveLevelViewport();

	if (const TSharedPtr<SLevelViewport> ActiveViewport = StaticCastSharedPtr<SLevelViewport>(ActiveViewportInterface); ActiveViewport.IsValid())
	{
		ActiveViewport->RemoveOverlayWidget(OverlayWidget.ToSharedRef());
		ActiveViewport->AddOverlayWidget(OverlayWidget.ToSharedRef());
	}
}

void FBDC_LevelSelectorModule::OnMapOpened(const FString& Filename, bool bAsTemplate)
{
	RefreshOverlay();
}
#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBDC_LevelSelectorModule, BDC_LevelSelector)