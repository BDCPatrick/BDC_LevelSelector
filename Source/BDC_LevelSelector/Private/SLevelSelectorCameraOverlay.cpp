/* Copyright © beginning at 2025 - BlackDevilCreations
* Author: Patrick Wenzel
* All rights reserved.
* This file and the corresponding Definition is part of a BlackDevilCreations project and may not be distributed, copied,
* or modified without prior written permission from BlackDevilCreations.
* Unreal Engine and its associated trademarks are property of Epic Games, Inc.
* and are used with permission.
*/
#include "SLevelSelectorCameraOverlay.h"
#include "BDC_LevelSelectorSettings.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Images/SImage.h"
#include "LevelEditorViewport.h"
#include "LevelEditor.h"
#include "Engine/World.h"
#include "Editor/EditorEngine.h"
#include "Editor.h"
#include "Misc/MessageDialog.h"
#include "SLevelViewport.h"
#include "Brushes/SlateRoundedBoxBrush.h"

#define LOCTEXT_NAMESPACE "SLevelSelectorCameraOverlay"

void SLevelSelectorCameraOverlay::Construct(const FArguments& InArgs)
{
	bIsCollapsed = false;

	BackgroundBrush = MakeShareable(new FSlateRoundedBoxBrush(FAppStyle::GetSlateColor("Colors.Panel"), 6.0f));

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(BackgroundBrush.Get())
			.Padding(5.0f)
			[
				SNew(SVerticalBox)
				
				+ SVerticalBox::Slot()
				.MaxHeight(28)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(5, 0, 5, 0)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("FavLabel", "Camera Favorites"))
						.Font(FAppStyle::GetFontStyle("NormalFontBold"))
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					]
					
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0)
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
						.OnClicked(this, &SLevelSelectorCameraOverlay::OnToggleCollapse)
						.ContentPadding(0)
						[
							SNew(SImage)
							.Image_Lambda([this]()
							{
								const FSlateBrush* IconBrush = nullptr;
								GetCollapseIcon(IconBrush);
								return IconBrush;
							})
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					.Visibility_Lambda([this]()
					{
						EVisibility Vis = EVisibility::Visible;
						GetContentVisibility(Vis);
						return Vis;
					})
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(0, 0, 2, 0)
						[
							SNew(SBox)
							.WidthOverride(100.0f)
							[
								SAssignNew(NameInputBox, SEditableTextBox)
								.HintText(LOCTEXT("NameHint", "New Name..."))
							]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(0, 0, 5, 0)
						[
							SNew(SButton)
							.Text(LOCTEXT("AddBtn", "+"))
							.OnClicked(this, &SLevelSelectorCameraOverlay::OnAddClicked)
							.ToolTipText(LOCTEXT("AddTooltip", "Add current view to favorites"))
							.ContentPadding(FMargin(5, 0))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(140.0f)
							[
								SAssignNew(FavComboButton, SComboButton)
								.OnGetMenuContent(this, &SLevelSelectorCameraOverlay::OnGetMenuContent)
								.IsEnabled(this, &SLevelSelectorCameraOverlay::IsFavListEnabled)
								.ButtonContent()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("SelectFav", "Select..."))
								]
							]
						]
					]
				]
			]
		]
	];
}

UBDC_LevelSelectorSettings* SLevelSelectorCameraOverlay::GetSettings() const
{
	return GetMutableDefault<UBDC_LevelSelectorSettings>();
}

void SLevelSelectorCameraOverlay::GetCurrentLevelSoftPtr(TSoftObjectPtr<UWorld>& OutLevelPtr) const
{
	OutLevelPtr = nullptr;
	if (GEditor && !GEditor->GetWorldContexts().IsEmpty())
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			OutLevelPtr = World;
		}
	}
}

bool SLevelSelectorCameraOverlay::IsFavListEnabled() const
{
	UBDC_LevelSelectorSettings* Settings = GetSettings();
	TSoftObjectPtr<UWorld> CurrentLevel;
	GetCurrentLevelSoftPtr(CurrentLevel);

	if (CurrentLevel.IsNull()) return false;
	if (!Settings->HoldFavorites.Contains(CurrentLevel)) return false;

	return Settings->HoldFavorites[CurrentLevel].HoldFavorites.Num() > 0;
}

FReply SLevelSelectorCameraOverlay::OnToggleCollapse()
{
	bIsCollapsed = !bIsCollapsed;
	return FReply::Handled();
}

void SLevelSelectorCameraOverlay::GetCollapseIcon(const FSlateBrush*& OutBrush) const
{
	if (bIsCollapsed)
	{
		OutBrush = FAppStyle::GetBrush("TreeArrow_Collapsed");
	}
	else
	{
		OutBrush = FAppStyle::GetBrush("TreeArrow_Expanded");
	}
}

void SLevelSelectorCameraOverlay::GetContentVisibility(EVisibility& OutVisibility) const
{
	OutVisibility = bIsCollapsed ? EVisibility::Collapsed : EVisibility::Visible;
}

FReply SLevelSelectorCameraOverlay::OnAddClicked()
{
	if (!NameInputBox.IsValid()) return FReply::Handled();

	const FText InputText = NameInputBox->GetText();
	if (InputText.ToString().Len() <= 4)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ShortNameErr", "Name must be longer than 4 characters."));
		return FReply::Handled();
	}

	FTransform CameraTransform = FTransform::Identity;
	bool bFoundViewport = false;

	if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");

		auto ActiveViewportInterface = LevelEditorModule.GetFirstActiveLevelViewport();

		if (const TSharedPtr<SLevelViewport> ActiveViewport = StaticCastSharedPtr<SLevelViewport>(ActiveViewportInterface); ActiveViewport.IsValid())
		{
			const FLevelEditorViewportClient& Client = ActiveViewport->GetLevelViewportClient();
			CameraTransform = FTransform(Client.GetViewRotation(), Client.GetViewLocation());
			bFoundViewport = true;
		}
	}

	if (!bFoundViewport) return FReply::Handled();

	UBDC_LevelSelectorSettings* Settings = GetSettings();
	TSoftObjectPtr<UWorld> CurrentLevel;
	GetCurrentLevelSoftPtr(CurrentLevel);

	if (CurrentLevel.IsNull()) return FReply::Handled();

	const FName NewName(*InputText.ToString());

	if (!Settings->HoldFavorites.Contains(CurrentLevel))
	{
		Settings->HoldFavorites.Add(CurrentLevel, FCameraFavorite());
	}

	Settings->HoldFavorites[CurrentLevel].HoldFavorites.Add(NewName, CameraTransform);
	Settings->SaveToProjectDefaultConfig();

	NameInputBox->SetText(FText::GetEmpty());

	return FReply::Handled();
}

TSharedRef<SWidget> SLevelSelectorCameraOverlay::OnGetMenuContent()
{
	TSharedRef<SVerticalBox> ListContainer = SNew(SVerticalBox);

	UBDC_LevelSelectorSettings* Settings = GetSettings();
	TSoftObjectPtr<UWorld> CurrentLevel;
	GetCurrentLevelSoftPtr(CurrentLevel);

	if (!CurrentLevel.IsNull() && Settings->HoldFavorites.Contains(CurrentLevel))
	{
		for (FCameraFavorite& Favs = Settings->HoldFavorites[CurrentLevel]; const auto& Elem : Favs.HoldFavorites)
		{
			const FName ItemName = Elem.Key;
			const FTransform ItemTrans = Elem.Value;

			ListContainer->AddSlot()
			             .AutoHeight()
			             .Padding(2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("NoBorder"))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "Menu.Button")
						.OnClicked(this, &SLevelSelectorCameraOverlay::OnTeleportClicked, ItemName, ItemTrans)
						.ContentPadding(FMargin(5.0f, 2.0f))
						[
							SNew(STextBlock)
							.Text(FText::FromName(ItemName))
							.Font(FAppStyle::GetFontStyle("NormalFont"))
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2.0f, 0.0f)
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "SimpleButton")
						.OnClicked(this, &SLevelSelectorCameraOverlay::OnEditItemClicked, ItemName)
						.ToolTipText(LOCTEXT("EditItemTip", "Rename Favorite"))
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("\xF040")))
							.Font(FAppStyle::GetFontStyle("FontAwesome.10"))
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2.0f, 0.0f)
					[
						SNew(SButton)
						.ButtonStyle(FAppStyle::Get(), "SimpleButton")
						.OnClicked(this, &SLevelSelectorCameraOverlay::OnDeleteItemClicked, ItemName)
						.ToolTipText(LOCTEXT("DeleteItemTip", "Delete Favorite"))
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("\xF014")))
							.Font(FAppStyle::GetFontStyle("FontAwesome.10"))
							.ColorAndOpacity(FLinearColor::Red)
						]
					]
				]
			];
		}
	}

	return SNew(SBox)
		.MaxDesiredHeight(300.0f)
		.WidthOverride(200.0f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				ListContainer
			]
		];
}

FReply SLevelSelectorCameraOverlay::OnTeleportClicked(FName Key, FTransform Transform)
{
	if (FavComboButton.IsValid()) FavComboButton->SetIsOpen(false);

	if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		const auto ActiveViewportInterface = LevelEditorModule.GetFirstActiveLevelViewport();

		if (const TSharedPtr<SLevelViewport> ActiveViewport = StaticCastSharedPtr<SLevelViewport>(ActiveViewportInterface); ActiveViewport.IsValid())
		{
			FLevelEditorViewportClient& Client = ActiveViewport->GetLevelViewportClient();
			Client.SetViewLocation(Transform.GetLocation());
			Client.SetViewRotation(Transform.GetRotation().Rotator());
			Client.Invalidate();
		}
	}

	return FReply::Handled();
}

FReply SLevelSelectorCameraOverlay::OnEditItemClicked(FName Key)
{
	if (FavComboButton.IsValid()) FavComboButton->SetIsOpen(false);

	TSharedPtr<SEditableTextBox> RenameInput;
	TSharedPtr<SWindow> PopupWindow;

	auto OnApply = [this, Key, &RenameInput, &PopupWindow]() -> FReply
	{
		if (RenameInput.IsValid())
		{
			const FName NewName(*RenameInput->GetText().ToString());
			if (!NewName.IsNone() && NewName != Key)
			{
				UBDC_LevelSelectorSettings* Settings = GetSettings();
				TSoftObjectPtr<UWorld> CurrentLevel;
				GetCurrentLevelSoftPtr(CurrentLevel);

				if (Settings->HoldFavorites.Contains(CurrentLevel))
				{
					if (Settings->HoldFavorites[CurrentLevel].HoldFavorites.Contains(Key))
					{
						const FTransform Trans = Settings->HoldFavorites[CurrentLevel].HoldFavorites[Key];
						Settings->HoldFavorites[CurrentLevel].HoldFavorites.Remove(Key);
						Settings->HoldFavorites[CurrentLevel].HoldFavorites.Add(NewName, Trans);

						Settings->SaveToProjectDefaultConfig();
					}
				}
			}
		}
		if (PopupWindow.IsValid()) PopupWindow->RequestDestroyWindow();
		return FReply::Handled();
	};

	auto OnCancel = [&PopupWindow]() -> FReply
	{
		if (PopupWindow.IsValid()) PopupWindow->RequestDestroyWindow();
		return FReply::Handled();
	};

	PopupWindow = SNew(SWindow)
		.Title(LOCTEXT("RenameTitle", "Rename Favorite"))
		.ClientSize(FVector2D(300, 100))
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(10)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 5, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("NewNameLbl", "New Name:"))
				]
				+ SHorizontalBox::Slot().FillWidth(1.0f)
				[
					SAssignNew(RenameInput, SEditableTextBox).Text(FText::FromName(Key))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10)
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 5, 0)
				[
					SNew(SButton).Text(LOCTEXT("ApplyBtn", "Apply")).OnClicked(FOnClicked::CreateLambda(OnApply))
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).Text(LOCTEXT("CancelBtn", "Cancel")).OnClicked(FOnClicked::CreateLambda(OnCancel))
				]
			]
		];

	FSlateApplication::Get().AddModalWindow(PopupWindow.ToSharedRef(), nullptr);

	return FReply::Handled();
}

FReply SLevelSelectorCameraOverlay::OnDeleteItemClicked(FName Key)
{
	if (FavComboButton.IsValid()) FavComboButton->SetIsOpen(false);

	if (const EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, FText::Format(LOCTEXT("ConfirmDelete", "Are you sure you want to delete '{0}'?"), FText::FromName(Key))); Result == EAppReturnType::Yes)
	{
		UBDC_LevelSelectorSettings* Settings = GetSettings();
		TSoftObjectPtr<UWorld> CurrentLevel;
		GetCurrentLevelSoftPtr(CurrentLevel);

		if (Settings->HoldFavorites.Contains(CurrentLevel))
		{
			Settings->HoldFavorites[CurrentLevel].HoldFavorites.Remove(Key);
			Settings->SaveToProjectDefaultConfig();
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE