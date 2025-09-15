#include "BDC_LevelSelector.h"
#include "SLevelSelectorComboBox.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FBDC_LevelSelectorModule"

void FBDC_LevelSelectorModule::StartupModule()
{
	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		
		ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Play", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw(this, &FBDC_LevelSelectorModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FBDC_LevelSelectorModule::ShutdownModule()
{
	if (ToolbarExtender.IsValid() && FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolbarExtender);
	}
}

void FBDC_LevelSelectorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddWidget(
		SAssignNew(LevelSelectorWidget, SLevelSelectorComboBox)
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBDC_LevelSelectorModule, BDC_LevelSelector)