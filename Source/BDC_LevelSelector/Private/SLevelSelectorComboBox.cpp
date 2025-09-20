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
#include "Styling/SlateTypes.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"

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


void SLevelSelectorComboBox::Construct(const FArguments& InArgs)
{
    DefaultLevelIcon = FAppStyle::GetBrush("LevelEditor.Tabs.Levels");
    RefreshIconBrush = FAppStyle::GetBrush("Icons.Refresh");

    // Load the favorite icons from the content folder and keep them resident to avoid checkerboard fallbacks
    FavoriteIconTextureTrue = LoadObject<UTexture2D>(nullptr, TEXT("/BDC_LevelSelector/Tex_MarkFav_True.Tex_MarkFav_True"));
    FavoriteIconTextureFalse = LoadObject<UTexture2D>(nullptr, TEXT("/BDC_LevelSelector/Tex_MarkFav_False.Tex_MarkFav_False"));

    // Default fallbacks
    FavoriteIconBrush = FAppStyle::GetBrush("Icons.Star");
    UnfavoriteIconBrush = FAppStyle::GetBrush("Icons.EmptyStar");

    if (FavoriteIconTextureTrue)
    {
        FavoriteIconTextureTrue->AddToRoot();
        FavoriteIconTextureTrue->SetForceMipLevelsToBeResident(30.0f);
        FavoriteIconTextureTrue->WaitForStreaming();

        FavoriteOwnedBrush = MakeUnique<FSlateBrush>();
        FavoriteOwnedBrush->SetResourceObject(FavoriteIconTextureTrue);
        FavoriteOwnedBrush->ImageSize = FVector2D(24, 24);
        FavoriteIconBrush = FavoriteOwnedBrush.Get();
    }

    if (FavoriteIconTextureFalse)
    {
        FavoriteIconTextureFalse->AddToRoot();
        FavoriteIconTextureFalse->SetForceMipLevelsToBeResident(30.0f);
        FavoriteIconTextureFalse->WaitForStreaming();

        UnfavoriteOwnedBrush = MakeUnique<FSlateBrush>();
        UnfavoriteOwnedBrush->SetResourceObject(FavoriteIconTextureFalse);
        UnfavoriteOwnedBrush->ImageSize = FVector2D(24, 24);
        UnfavoriteIconBrush = UnfavoriteOwnedBrush.Get();
    }

    HeaderItem = MakeShareable(new FLevelSelectorItem(FAssetData()));

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
             .OnComboBoxOpening(this, &SLevelSelectorComboBox::OnComboBoxOpening)
             .MaxListHeight(480.0f)
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
          .Padding(FMargin(4, 0, 0, 0))
          [
             SNew(SBox)
             .WidthOverride(28)
             .HeightOverride(28)
             [
                SNew(SOverlay)
                
                +SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                  SNew(SButton)
                  .OnClicked(this, &SLevelSelectorComboBox::OnRefreshButtonClicked)
                  .ContentPadding(0)
                   [
                      SNew(STextBlock)
                      .Text(FText::FromString(TEXT("")))
                   ]
                ]
                +SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .Padding(4.0f)
                [
                   SNew(SImage)
                   .Image(RefreshIconBrush)
                   .DesiredSizeOverride(FVector2D(20, 20))
                   .Visibility(EVisibility::HitTestInvisible)
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
       EnsureSelectedCurrentLevel(true);
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
    FEditorDelegates::OnMapOpened.RemoveAll(this);

    FavoriteIconTextureTrue = nullptr;
    FavoriteIconTextureFalse = nullptr;
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
             ComboBoxContentContainer->SetContent(CreateSelectedItemWidget(Item));
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
    AllLevels.Empty();

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
             AllLevels.Add(FLevelSelectorItem::Create(AssetData));
          }
       }
    }

    for (const FAssetData& Data : AssetDataList)
    {
       if (Data.GetSoftObjectPath().GetLongPackageName().StartsWith(TEXT("/Game/")))
       {
          TSharedRef<FLevelSelectorItem> NewItem = FLevelSelectorItem::Create(Data);
          bool bIsAlreadyInList = false;
          for (const TSharedPtr<FLevelSelectorItem>& ExistingItem : AllLevels)
          {
             if (ExistingItem->PackagePath == NewItem->PackagePath)
             {
                bIsAlreadyInList = true;
                break;
             }
          }
          if (!bIsAlreadyInList)
          {
             AllLevels.Add(NewItem);
          }
       }
    }

    SortLevelList();

    ApplyFilters();
}

void SLevelSelectorComboBox::SortLevelList()
{
    if (const UBDC_LevelSelectorSettings* Settings = GetDefault<UBDC_LevelSelectorSettings>())
    {
       AllLevels.Sort([&](const TSharedPtr<FLevelSelectorItem>& A, const TSharedPtr<FLevelSelectorItem>& B)
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
       AllLevels.Sort([](const TSharedPtr<FLevelSelectorItem>& A, const TSharedPtr<FLevelSelectorItem>& B)
       {
          return A->PackagePath < B->PackagePath;
       });
    }
}

TSharedRef<SWidget> SLevelSelectorComboBox::OnGenerateComboWidget(TSharedPtr<FLevelSelectorItem> InItem)
{
    if (IsHeaderItem(InItem))
    {
        return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(FMargin(4.0f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(FMargin(0,0,4,0))
            [
                SAssignNew(SearchTextBoxWidget, SEditableTextBox)
                .HintText(FText::FromString(TEXT("Search levels...")))
                .Text(SearchTextFilter)
                .OnTextChanged(this, &SLevelSelectorComboBox::OnSearchTextChanged)
                .OnTextCommitted(this, &SLevelSelectorComboBox::OnSearchTextCommitted)
                .MinDesiredWidth(160)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(FMargin(0,0,4,0))
            [
                SAssignNew(FilterTagComboWidget, SGameplayTagCombo)
                .OnTagChanged_Lambda([this](const FGameplayTag NewTag)
                {
                    OnFilterTagChanged(NewTag);
                })
                .Tag(SelectedFilterTag)
                .Filter(FString())
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), "SimpleButton")
                .OnClicked(this, &SLevelSelectorComboBox::OnClearFilterClicked)
                .ToolTipText(FText::FromString(TEXT("Clear search and tag filter")))
                [
                    SNew(STextBlock).Text(FText::FromString(TEXT("X")))
                ]
            ]
        ];
    }
    return CreateLevelItemWidget(InItem);
}

void SLevelSelectorComboBox::OnSelectionChanged(TSharedPtr<FLevelSelectorItem> InItem, ESelectInfo::Type SelectInfo)
{
    if (IsHeaderItem(InItem))
    {
        // Never act on header row selection
        if (LevelComboBox.IsValid())
        {
            LevelComboBox->ClearSelection();
        }
        return;
    }

    if (SelectInfo != ESelectInfo::OnMouseClick && SelectInfo != ESelectInfo::OnKeyPress)
    {
       return;
    }
    if (InItem.IsValid())
    {
       FEditorFileUtils::LoadMap(InItem->AssetData.GetSoftObjectPath().ToString());
    }
}

TSharedRef<SWidget> SLevelSelectorComboBox::CreateSelectedItemWidget(const TSharedPtr<FLevelSelectorItem>& InItem)
{
    if (!InItem.IsValid())
    {
       return SNew(STextBlock).Text(FText::FromString(TEXT("Invalid Level")));
    }

    const FSlateBrush* FinalBrush = DefaultLevelIcon;
    
    const UBDC_LevelSelectorSettings* Settings = GetDefault<UBDC_LevelSelectorSettings>();
    FString TagString = TEXT("");
    if (Settings)
    {
       if (const FGameplayTag* FoundTag = Settings->LevelTags.Find(InItem->AssetData.GetSoftObjectPath()); FoundTag && FoundTag->IsValid())
        {
            FString TagName = FoundTag->ToString();
            TArray<FString> TagParts;
            TagName.ParseIntoArray(TagParts, TEXT("."), true);

            if (TagParts.Num() > 2)
            {
                TagString = FString::Printf(TEXT(" (...%s.%s)"), *TagParts[TagParts.Num() - 2], *TagParts.Last());
            }
            else
            {
                TagString = FString::Printf(TEXT(" (%s)"), *TagName);
            }
        }
    }

    const FString DisplayText = InItem->DisplayName + TagString;

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
          .Text(FText::FromString(DisplayText))
          .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
       ]
       + SHorizontalBox::Slot()
       .AutoWidth()
       .HAlign(HAlign_Right)
       .VAlign(VAlign_Center)
       .Padding(4.0f, 3.0f, 3.0f, 3.0f)
       [
           SNew(SBox)
           .WidthOverride(18)
           .HeightOverride(18)
           .Visibility(InItem->IsFavorite() ? EVisibility::Visible : EVisibility::Collapsed)
           [
               SNew(SImage)
               .Image(FavoriteIconBrush)
               .DesiredSizeOverride(FVector2D(18, 18))
           ]
       ];
}

TSharedRef<SWidget> SLevelSelectorComboBox::CreateLevelItemWidget(const TSharedPtr<FLevelSelectorItem>& InItem)
{
    if (!InItem.IsValid())
    {
       return SNew(STextBlock).Text(FText::FromString(TEXT("Invalid Level")));
    }

    const FSlateBrush* FinalBrush = DefaultLevelIcon;

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
       .MinWidth(200)
       .Padding(4.0f, 2.0f)
       [
          SNew(STextBlock)
          .Text(FText::FromString(InItem->DisplayName))
          .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
          .MinDesiredWidth(200)
          .Clipping(EWidgetClipping::ClipToBounds)
       ]
       + SHorizontalBox::Slot()
       .AutoWidth()
       .HAlign(HAlign_Right)
       .VAlign(VAlign_Center)
       .Padding(4.0f, 0.0f, 0.0f, 0.0f)
       .MaxWidth(200)
       [
          CreateTagSelectionWidget(InItem)
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

bool SLevelSelectorComboBox::IsHeaderItem(const TSharedPtr<FLevelSelectorItem>& InItem) const
{
    return HeaderItem.IsValid() && InItem == HeaderItem;
}

FGameplayTag SLevelSelectorComboBox::GetItemTag(const TSharedPtr<FLevelSelectorItem>& InItem) const
{
    if (!InItem.IsValid())
    {
        return FGameplayTag();
    }
    if (const UBDC_LevelSelectorSettings* Settings = GetDefault<UBDC_LevelSelectorSettings>())
    {
        if (const FGameplayTag* Found = Settings->LevelTags.Find(InItem->AssetData.GetSoftObjectPath()))
        {
            return *Found;
        }
    }
    return FGameplayTag();
}

void SLevelSelectorComboBox::ApplyFilters()
{
    const FString SearchLower = SearchTextFilter.ToString().ToLower();

    LevelListSource.Empty();
	
    if (HeaderItem.IsValid())
    {
        LevelListSource.Add(HeaderItem);
    }

    for (const TSharedPtr<FLevelSelectorItem>& Item : AllLevels)
    {
        if (!Item.IsValid()) continue;

        bool bPassSearch = true;
        if (!SearchLower.IsEmpty())
        {
            bPassSearch = Item->DisplayName.ToLower().Contains(SearchLower);
        }

        bool bPassTag = true;
        if (SelectedFilterTag.IsValid())
        {
            bPassTag = GetItemTag(Item) == SelectedFilterTag;
        }

        if (bPassSearch && bPassTag)
        {
            LevelListSource.Add(Item);
        }
    }

    if (LevelComboBox.IsValid())
    {
        LevelComboBox->RefreshOptions();
    }
}

void SLevelSelectorComboBox::OnSearchTextChanged(const FText& InText)
{
    SearchTextFilter = InText;
    ApplyFilters();
}

void SLevelSelectorComboBox::OnSearchTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
    SearchTextFilter = InText;
    ApplyFilters();
}

void SLevelSelectorComboBox::OnFilterTagChanged(FGameplayTag InTag)
{
    SelectedFilterTag = InTag;
    ApplyFilters();
}

FReply SLevelSelectorComboBox::OnClearFilterClicked()
{
    SearchTextFilter = FText::GetEmpty();
    SelectedFilterTag = FGameplayTag();

    if (SearchTextBoxWidget.IsValid())
    {
        SearchTextBoxWidget->SetText(SearchTextFilter);
    }
    if (FilterTagComboWidget.IsValid())
    {
    	
    }

    ApplyFilters();
    return FReply::Handled();
}
