#include "SLevelSelectorComboBox.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SImage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "Styling/AppStyle.h"

FLevelSelectorItem::FLevelSelectorItem(const FAssetData& InAssetData) : AssetData(InAssetData)
{
	DisplayName = AssetData.AssetName.ToString();
	PackagePath = AssetData.GetSoftObjectPath().GetLongPackageName();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLevelSelectorComboBox::Construct(const FArguments& InArgs)
{
	DefaultLevelIcon = FAppStyle::GetBrush("LevelEditor.Tabs.Levels");
	PopulateLevelList();

	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(12.0, 2.0))
		.HeightOverride(32)
		.MinDesiredWidth(128)
		.MaxDesiredWidth(256)
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
				[
					SAssignNew(ComboBoxContentContainer, SBox)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("Select a Level...")))
					]
				]
			]
		]
	];

	FEditorDelegates::OnMapOpened.AddSP(this, &SLevelSelectorComboBox::HandleMapOpened);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnFilesLoaded().AddSP(this, &SLevelSelectorComboBox::OnAssetRegistryFilesLoaded);
}

SLevelSelectorComboBox::~SLevelSelectorComboBox()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().OnFilesLoaded().RemoveAll(this);
	}
	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		FEditorDelegates::OnMapOpened.RemoveAll(this);
	}
}

void SLevelSelectorComboBox::OnAssetRegistryFilesLoaded()
{
	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		RefreshSelection(GEditor->GetEditorWorldContext().World()->GetPathName());
	}
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnFilesLoaded().RemoveAll(this);
}

void SLevelSelectorComboBox::RefreshSelection(const FString& MapPath)
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

	if(LevelComboBox.IsValid())
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

void SLevelSelectorComboBox::PopulateLevelList()
{
	LevelListSource.Empty();
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass(UWorld::StaticClass()->GetClassPathName(), AssetDataList);

	for (const FAssetData& Data : AssetDataList)
	{
		if (Data.GetSoftObjectPath().GetLongPackageName().StartsWith(TEXT("/Game/")))
		{
			LevelListSource.Add(FLevelSelectorItem::Create(Data));
		}
	}

	LevelListSource.Sort([](const TSharedPtr<FLevelSelectorItem>& A, const TSharedPtr<FLevelSelectorItem>& B)
	{
		return A->PackagePath < B->PackagePath;
	});
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

TSharedRef<SWidget> SLevelSelectorComboBox::CreateLevelItemWidget(TSharedPtr<FLevelSelectorItem> InItem) const
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
		];
}

void SLevelSelectorComboBox::HandleMapOpened(const FString& Filename, bool bAsTemplate)
{
	RefreshSelection(Filename);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION