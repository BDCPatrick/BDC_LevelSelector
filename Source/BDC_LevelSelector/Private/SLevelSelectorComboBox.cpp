#include "SLevelSelectorComboBox.h"
#include "BDC_LevelSelectorSettings.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "GameplayTagContainer.h"
#include "IContentBrowserSingleton.h"
#include "SGameplayTagCombo.h"
#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Modules/ModuleManager.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWidget.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

FLevelSelectorItem::FLevelSelectorItem(const FAssetData& InAssetData) : AssetData(InAssetData)
{
	DisplayName = AssetData.AssetName.ToString();
	PackagePath = InAssetData.GetSoftObjectPath().GetLongPackageName();
}

bool FLevelSelectorItem::IsFavorite() const
{
	if (const UBDC_LevelSelectorSettings* Settings = GetDefault<UBDC_LevelSelectorSettings>())
	{
		const FSoftObjectPath Path = AssetData.GetSoftObjectPath();
		return Settings->FavoriteLevels.Contains(Path);
	}
	return false;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLevelSelectorComboBox::Construct(const FArguments& InArgs)
{
	DefaultLevelIcon = FAppStyle::GetBrush("LevelEditor.Tabs.Levels");
	RefreshIconBrush = FAppStyle::GetBrush("Icons.Refresh");

	// Load the favorite icons from the content folder
	UTexture2D* TexTrue = LoadObject<UTexture2D>(nullptr, TEXT("/BDC_LevelSelector/Tex_MarkFav_True.Tex_MarkFav_True"));
	UTexture2D* TexFalse = LoadObject<UTexture2D>(nullptr, TEXT("/BDC_LevelSelector/Tex_MarkFav_False.Tex_MarkFav_False"));
	
	FavoriteIconBrush = TexTrue ? new FSlateImageBrush(TexTrue, FVector2D(24, 24)) : FAppStyle::GetBrush("Icons.Star");
	UnfavoriteIconBrush = TexFalse ? new FSlateImageBrush(TexFalse, FVector2D(24, 24)) : FAppStyle::GetBrush("Icons.EmptyStar");

	PopulateLevelList();

	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(12.0, 2.0))
		.HeightOverride(32)
		.MinDesiredWidth(320)
		.MaxDesiredWidth(480)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 8, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Level:")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SAssignNew(LevelComboBox, SComboBox<TSharedPtr<FLevelSelectorItem>>)
				.OptionsSource(&LevelListSource)
				.OnGenerateWidget(this, &SLevelSelectorComboBox::OnGenerateComboWidget)
				.OnSelectionChanged(this, &SLevelSelectorComboBox::OnSelectionChanged)
				.OnComboBoxOpening(this, &SLevelSelectorComboBox::OnComboBoxOpening) // WIRD BEIM ÖFFNEN AUFGERUFEN
				[
					SAssignNew(ComboBoxContentContainer, SBox)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("Select a Level...")))
					]
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(8.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(28)
				.HeightOverride(28)
				[
					SNew(SButton)
					.OnClicked(this, &SLevelSelectorComboBox::OnRefreshButtonClicked)
					.ContentPadding(0)
					[
						SNew(SImage)
						.Image(RefreshIconBrush)
					]
				]
			]
		]
	];

	FEditorDelegates::OnMapOpened.AddSP(this, &SLevelSelectorComboBox::HandleMapOpened);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnFilesLoaded().AddSP(this, &SLevelSelectorComboBox::OnAssetRegistryFilesLoaded);

	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		EnsureSelectedCurrentLevel(/*bStrict*/ true);
	}
}

void SLevelSelectorComboBox::OnComboBoxOpening()
{
	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		EnsureSelectedCurrentLevel(true);
	}
}


SLevelSelectorComboBox::~SLevelSelectorComboBox()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
	{
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().OnFilesLoaded().RemoveAll(this);
	}
	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		FEditorDelegates::OnMapOpened.RemoveAll(this);
	}
}

void SLevelSelectorComboBox::OnAssetRegistryFilesLoaded()
{
	if (UBDC_LevelSelectorSettings* MutableSettings = GetMutableDefault<UBDC_LevelSelectorSettings>())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		TSet<FSoftObjectPath> FavoriteLevelsToRemove;
		for (const FSoftObjectPath& FavoritePath : MutableSettings->FavoriteLevels)
		{
			if (FavoritePath.IsValid())
			{
				const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FavoritePath);
				if (!AssetData.IsValid())
				{
					FavoriteLevelsToRemove.Add(FavoritePath);
				}
			}
		}
		
		TArray<FSoftObjectPath> HoldKeys;
		MutableSettings->LevelTags.GetKeys(HoldKeys);
		for (const FSoftObjectPath& KeyPath : HoldKeys)
		{
			if (KeyPath.IsValid())
			{
				const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(KeyPath);
				if (!AssetData.IsValid())
				{
					MutableSettings->LevelTags.Remove(KeyPath);
				}
			}
		}

		for (const FSoftObjectPath& L : FavoriteLevelsToRemove)
		{
			MutableSettings->FavoriteLevels.Remove(L);
		}
	}
	
	PopulateLevelList();
	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		RefreshSelection(GEditor->GetEditorWorldContext().World()->GetPathName(), true);
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnFilesLoaded().RemoveAll(this);
}

void SLevelSelectorComboBox::RefreshSelection(const FString& MapPath, bool bStrict)
{
	const FString PackagePath = FPackageName::ObjectPathToPackageName(MapPath);

	for (const auto& Item : LevelListSource)
	{
		if (Item.IsValid() && Item->PackagePath.Equals(PackagePath, ESearchCase::IgnoreCase))
		{
			if (LevelComboBox.IsValid())
			{
				LevelComboBox->SetSelectedItem(Item);
			}
			if (ComboBoxContentContainer.IsValid())
			{
				ComboBoxContentContainer->SetContent(CreateLevelItemWidget(Item));
			}
			return;
		}
	}

	if (!bStrict)
	{
		return;
	}

	if (LevelComboBox.IsValid())
	{
		LevelComboBox->ClearSelection();
	}
	if (ComboBoxContentContainer.IsValid())
	{
		ComboBoxContentContainer->SetContent(
			SNew(STextBlock).Text(FText::FromString(TEXT("Select a Level...")))
		);
	}
}

void SLevelSelectorComboBox::EnsureSelectedCurrentLevel(bool bStrict)
{
	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		const FString CurrentMapPath = GEditor->GetEditorWorldContext().World()->GetPathName();
		RefreshSelection(CurrentMapPath, bStrict);
	}
}

void SLevelSelectorComboBox::PopulateLevelList()
{
	LevelListSource.Empty();

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass(UWorld::StaticClass()->GetClassPathName(), AssetDataList);

	const UBDC_LevelSelectorSettings* Settings = GetDefault<UBDC_LevelSelectorSettings>();
	if (!Settings)
	{
		return;
	}

	for (const FSoftObjectPath& FavoritePath : Settings->FavoriteLevels)
	{
		if (FavoritePath.IsValid())
		{
			if (const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FavoritePath); AssetData.IsValid())
			{
				LevelListSource.Add(FLevelSelectorItem::Create(AssetData));
			}
		}
	}

	for (const FAssetData& Data : AssetDataList)
	{
		if (Data.GetSoftObjectPath().GetLongPackageName().StartsWith(TEXT("/Game/")))
		{
			TSharedRef<FLevelSelectorItem> NewItem = FLevelSelectorItem::Create(Data);
			bool bIsAlreadyInList = false;
			for (const TSharedPtr<FLevelSelectorItem>& ExistingItem : LevelListSource)
			{
				if (ExistingItem->PackagePath == NewItem->PackagePath)
				{
					bIsAlreadyInList = true;
					break;
				}
			}
			if (!bIsAlreadyInList)
			{
				LevelListSource.Add(NewItem);
			}
		}
	}

	SortLevelList();

	if (LevelComboBox.IsValid())
	{
		LevelComboBox->RefreshOptions();
	}
}

void SLevelSelectorComboBox::SortLevelList()
{
	if (const UBDC_LevelSelectorSettings* Settings = GetDefault<UBDC_LevelSelectorSettings>())
	{
		LevelListSource.Sort([&](const TSharedPtr<FLevelSelectorItem>& A, const TSharedPtr<FLevelSelectorItem>& B)
		{
			const FSoftObjectPath PathA = A->AssetData.GetSoftObjectPath();
			const FSoftObjectPath PathB = B->AssetData.GetSoftObjectPath();

			const bool bAIsFavorite = Settings->FavoriteLevels.Contains(PathA);

			if (const bool bBIsFavorite = Settings->FavoriteLevels.Contains(PathB); bAIsFavorite != bBIsFavorite)
			{
				return bAIsFavorite;
			}
			return A->PackagePath < B->PackagePath;
		});
	}
	else
	{
		LevelListSource.Sort([](const TSharedPtr<FLevelSelectorItem>& A, const TSharedPtr<FLevelSelectorItem>& B)
		{
			return A->PackagePath < B->PackagePath;
		});
	}
}

TSharedRef<SWidget> SLevelSelectorComboBox::OnGenerateComboWidget(TSharedPtr<FLevelSelectorItem> InItem)
{
	return CreateLevelItemWidget(InItem);
}

void SLevelSelectorComboBox::OnSelectionChanged(TSharedPtr<FLevelSelectorItem> InItem, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo != ESelectInfo::OnMouseClick && SelectInfo != ESelectInfo::OnKeyPress)
	{
		return;
	}
	if (InItem.IsValid())
	{
		FEditorFileUtils::LoadMap(InItem->AssetData.GetSoftObjectPath().ToString());
	}
}

TSharedRef<SWidget> SLevelSelectorComboBox::CreateLevelItemWidget(const TSharedPtr<FLevelSelectorItem>& InItem)
{
	if (!InItem.IsValid())
	{
		return SNew(STextBlock).Text(FText::FromString(TEXT("Invalid Level")));
	}

	const TSharedPtr<FSlateBrush> ThumbnailBrush = MakeShareable(new FSlateDynamicImageBrush(
		FName(*InItem->AssetData.GetSoftObjectPath().ToString()),
		FVector2D(24, 24)
	));

	const FSlateBrush* FinalBrush = ThumbnailBrush->GetResourceObject() != nullptr ? ThumbnailBrush.Get() : DefaultLevelIcon;

	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(24)
			.HeightOverride(24)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0.0f, 2.0f)
			[
				SNew(SImage)
				.Image(FinalBrush)
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 2.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InItem->DisplayName))
			.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 0.0f, 0.0f, 0.0f)
		[
			SNew(SBox)
			.WidthOverride(18)
			.HeightOverride(18)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "NoBorder")
				.OnClicked(FOnClicked::CreateLambda([this, Item = InItem]() mutable -> FReply
				{
					return OnShowInContentBrowserClicked(Item).Handled();
				}))
				.ContentPadding(2)
				.ToolTipText(FText::FromString(TEXT("Show in Content Browser")))
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("SystemWideCommands.FindInContentBrowser"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 0.0f, 0.0f, 0.0f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.OnClicked(FOnClicked::CreateLambda([this, Item = InItem]() -> FReply
			{
				const ECheckBoxState NewState = Item->IsFavorite() ? ECheckBoxState::Unchecked : ECheckBoxState::Checked;
				OnFavoriteCheckboxChanged(NewState, Item);
				return FReply::Handled();
			}))
			.ToolTipText(FText::FromString(TEXT("Add/Remove from favorites")))
			.Content()
			[
				SNew(SImage)
				.Image(InItem->IsFavorite() ? FavoriteIconBrush : UnfavoriteIconBrush)
				.DesiredSizeOverride(FVector2D(24, 24))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 0.0f, 0.0f, 0.0f)
		[
			CreateTagSelectionWidget(InItem)
		];
}

TSharedRef<SWidget> SLevelSelectorComboBox::CreateTagSelectionWidget(const TSharedPtr<FLevelSelectorItem>& InItem)
{
	const FSoftObjectPath LevelPath(InItem->AssetData.GetSoftObjectPath());
	UBDC_LevelSelectorSettings* Settings = GetMutableDefault<UBDC_LevelSelectorSettings>();

	const FGameplayTag* CurrentTag = Settings->LevelTags.Find(LevelPath);

	return SNew(SComboButton)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(CurrentTag ? CurrentTag->ToString() : TEXT("No Tag")))
		]
		.MenuContent()
		[
			SNew(SBox)
			.MaxDesiredHeight(400)
			.WidthOverride(300)
			[
				SNew(SGameplayTagCombo)
				.OnTagChanged_Lambda([this, Item = InItem](const FGameplayTag NewTag) mutable
				{
					OnTagChanged(Item, NewTag);
				})
				.Tag(CurrentTag ? *CurrentTag : FGameplayTag())
				.Filter(FString())
			]
		];
}

void SLevelSelectorComboBox::OnTagChanged(const TSharedPtr<FLevelSelectorItem>& InItem, FGameplayTag NewTag)
{
	if (!InItem.IsValid())
	{
		return;
	}

	if (UBDC_LevelSelectorSettings* LocaleSettings = GetMutableDefault<UBDC_LevelSelectorSettings>())
	{
		if (UWorld* LevelObject = TSoftObjectPtr<UWorld>(InItem->AssetData.GetSoftObjectPath()).LoadSynchronous())
		{
			LocaleSettings->SetLevelTag(LevelObject, NewTag);

			PopulateLevelList();

			if (LevelComboBox.IsValid() && LevelComboBox->IsOpen())
			{
				LevelComboBox->SetIsOpen(false);
			}
			EnsureSelectedCurrentLevel(true);
		}
	}
}

void SLevelSelectorComboBox::OnFavoriteCheckboxChanged(ECheckBoxState NewState, TSharedPtr<FLevelSelectorItem> InItem)
{
	if (!InItem.IsValid())
	{
		return;
	}

	const TSoftObjectPtr<UWorld> LevelSoftObjectPath(InItem->AssetData.GetSoftObjectPath());
	if (UWorld* LevelObject = LevelSoftObjectPath.LoadSynchronous())
	{
		if (UBDC_LevelSelectorSettings* Settings = GetMutableDefault<UBDC_LevelSelectorSettings>())
		{
			if (NewState == ECheckBoxState::Checked) { Settings->AddFavorite(LevelObject); }
			else { Settings->RemoveFavorite(LevelObject); }

			PopulateLevelList();

			if (LevelComboBox.IsValid() && LevelComboBox->IsOpen())
			{
				LevelComboBox->SetIsOpen(false);
			}
			EnsureSelectedCurrentLevel(true);
		}
	}
}

void SLevelSelectorComboBox::HandleMapOpened(const FString& Filename, bool bAsTemplate)
{
	RefreshSelection(Filename, true);
}

FReply SLevelSelectorComboBox::OnRefreshButtonClicked()
{
	PopulateLevelList();
	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		RefreshSelection(GEditor->GetEditorWorldContext().World()->GetPathName(), true);
	}
	return FReply::Handled();
}

FReply SLevelSelectorComboBox::OnShowInContentBrowserClicked(const TSharedPtr<FLevelSelectorItem>& InItem) const
{
	if (InItem.IsValid())
	{
		const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		TArray<FAssetData> AssetsToSelect;
		AssetsToSelect.Add(InItem->AssetData);
		ContentBrowserModule.Get().SyncBrowserToAssets(AssetsToSelect);

		if (LevelComboBox.IsValid())
		{
			LevelComboBox->SetIsOpen(false);
		}

		const_cast<SLevelSelectorComboBox*>(this)->EnsureSelectedCurrentLevel(true);
	}
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
