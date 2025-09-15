#include "SLevelSelectorComboBox.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SImage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "FileHelpers.h"

FLevelSelectorItem::FLevelSelectorItem(const FAssetData& InAssetData): AssetData(InAssetData)
{
	DisplayName = AssetData.AssetName.ToString();
	PackagePath = AssetData.GetSoftObjectPath().GetLongPackageName();

	ThumbnailBrush = MakeShareable(new FSlateDynamicImageBrush(
		FName(*AssetData.GetSoftObjectPath().ToString()),
		FVector2D(40, 40)
	));
}


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLevelSelectorComboBox::Construct(const FArguments& InArgs)
{
	PopulateLevelList();
	
	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(48)
		.MinDesiredWidth(128)
		.MaxDesiredWidth(256)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4.0f, 0.0f)
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
					SNew(SBox)
					.Padding(FMargin(4, 0))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							(LevelComboBox.IsValid() && LevelComboBox->GetSelectedItem().IsValid())
							? CreateSelectedItemWidget(LevelComboBox->GetSelectedItem())
							: SNullWidget::NullWidget
						]
					]
				]
			]
		]
	];

	FEditorDelegates::OnMapOpened.AddSP(this, &SLevelSelectorComboBox::HandleMapOpened);

	if (GEditor && GEditor->GetEditorWorldContext().World())
	{
		RefreshSelection(GEditor->GetEditorWorldContext().World()->GetPathName());
	}
}

SLevelSelectorComboBox::~SLevelSelectorComboBox()
{
	if (FEditorDelegates::OnMapOpened.IsBoundToObject(this))
	{
		FEditorDelegates::OnMapOpened.RemoveAll(this);
	}
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
				LevelComboBox->RefreshOptions();
			}
			return;
		}
	}
	
	if(LevelComboBox.IsValid())
	{
		LevelComboBox->ClearSelection();
	}
}

void SLevelSelectorComboBox::PopulateLevelList()
{
	LevelListSource.Empty();
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;

	AssetRegistryModule.Get().GetAssetsByClass(UWorld::StaticClass()->GetClassPathName(), AssetData);

	for (const FAssetData& Data : AssetData)
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

TSharedRef<SWidget> SLevelSelectorComboBox::OnGenerateComboWidget(TSharedPtr<FLevelSelectorItem> InItem) const
{
	return CreateSelectedItemWidget(InItem);
}

void SLevelSelectorComboBox::OnSelectionChanged(TSharedPtr<FLevelSelectorItem> InItem, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo != ESelectInfo::OnMouseClick && SelectInfo != ESelectInfo::OnKeyPress)
	{
		return;
	}

	if (InItem.IsValid())
	{
		const FString AssetPath = InItem->AssetData.GetSoftObjectPath().ToString();
		FEditorFileUtils::LoadMap(AssetPath);
	}
}

TSharedRef<SWidget> SLevelSelectorComboBox::CreateSelectedItemWidget(TSharedPtr<FLevelSelectorItem> InItem) const
{
	if (!InItem.IsValid())
	{
		return SNew(STextBlock).Text(FText::FromString(TEXT("Select a Level...")));
	}
	
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(40)
			.HeightOverride(40)
			[
				SNew(SImage)
				.Image(InItem->ThumbnailBrush.Get())
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 0.0f)
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