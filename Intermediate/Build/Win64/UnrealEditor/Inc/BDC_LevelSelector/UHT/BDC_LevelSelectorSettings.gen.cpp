// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BDC_LevelSelectorSettings.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeBDC_LevelSelectorSettings() {}

// ********** Begin Cross Module References ********************************************************
BDC_LEVELSELECTOR_API UClass* Z_Construct_UClass_UBDC_LevelSelectorSettings();
BDC_LEVELSELECTOR_API UClass* Z_Construct_UClass_UBDC_LevelSelectorSettings_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FSoftObjectPath();
DEVELOPERSETTINGS_API UClass* Z_Construct_UClass_UDeveloperSettings();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
UPackage* Z_Construct_UPackage__Script_BDC_LevelSelector();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UBDC_LevelSelectorSettings ***********************************************
void UBDC_LevelSelectorSettings::StaticRegisterNativesUBDC_LevelSelectorSettings()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UBDC_LevelSelectorSettings;
UClass* UBDC_LevelSelectorSettings::GetPrivateStaticClass()
{
	using TClass = UBDC_LevelSelectorSettings;
	if (!Z_Registration_Info_UClass_UBDC_LevelSelectorSettings.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("BDC_LevelSelectorSettings"),
			Z_Registration_Info_UClass_UBDC_LevelSelectorSettings.InnerSingleton,
			StaticRegisterNativesUBDC_LevelSelectorSettings,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UBDC_LevelSelectorSettings.InnerSingleton;
}
UClass* Z_Construct_UClass_UBDC_LevelSelectorSettings_NoRegister()
{
	return UBDC_LevelSelectorSettings::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "BDC_LevelSelectorSettings.h" },
		{ "ModuleRelativePath", "Public/BDC_LevelSelectorSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FavoriteLevels_MetaData[] = {
		{ "Category", "Level Selector" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** A set of favorite levels. These levels will always appear at the top of the list. */" },
#endif
		{ "ModuleRelativePath", "Public/BDC_LevelSelectorSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A set of favorite levels. These levels will always appear at the top of the list." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LevelTags_MetaData[] = {
		{ "Category", "Level Selector" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** A set of favorite levels. These levels will always appear at the top of the list. */" },
#endif
		{ "ModuleRelativePath", "Public/BDC_LevelSelectorSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A set of favorite levels. These levels will always appear at the top of the list." },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_FavoriteLevels_ElementProp;
	static const UECodeGen_Private::FSetPropertyParams NewProp_FavoriteLevels;
	static const UECodeGen_Private::FStructPropertyParams NewProp_LevelTags_ValueProp;
	static const UECodeGen_Private::FStructPropertyParams NewProp_LevelTags_Key_KeyProp;
	static const UECodeGen_Private::FMapPropertyParams NewProp_LevelTags;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBDC_LevelSelectorSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_FavoriteLevels_ElementProp = { "FavoriteLevels", nullptr, (EPropertyFlags)0x0000000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FSoftObjectPath, METADATA_PARAMS(0, nullptr) };
static_assert(TModels_V<CGetTypeHashable, FSoftObjectPath>, "The structure 'FSoftObjectPath' is used in a TSet but does not have a GetValueTypeHash defined");
const UECodeGen_Private::FSetPropertyParams Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_FavoriteLevels = { "FavoriteLevels", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Set, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBDC_LevelSelectorSettings, FavoriteLevels), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FavoriteLevels_MetaData), NewProp_FavoriteLevels_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_LevelTags_ValueProp = { "LevelTags", nullptr, (EPropertyFlags)0x0000000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 1, Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(0, nullptr) }; // 133831994
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_LevelTags_Key_KeyProp = { "LevelTags_Key", nullptr, (EPropertyFlags)0x0000000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FSoftObjectPath, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FMapPropertyParams Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_LevelTags = { "LevelTags", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBDC_LevelSelectorSettings, LevelTags), EMapPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LevelTags_MetaData), NewProp_LevelTags_MetaData) }; // 133831994
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_FavoriteLevels_ElementProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_FavoriteLevels,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_LevelTags_ValueProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_LevelTags_Key_KeyProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::NewProp_LevelTags,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UDeveloperSettings,
	(UObject* (*)())Z_Construct_UPackage__Script_BDC_LevelSelector,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::ClassParams = {
	&UBDC_LevelSelectorSettings::StaticClass,
	"Editor",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::PropPointers),
	0,
	0x001000A6u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UBDC_LevelSelectorSettings()
{
	if (!Z_Registration_Info_UClass_UBDC_LevelSelectorSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBDC_LevelSelectorSettings.OuterSingleton, Z_Construct_UClass_UBDC_LevelSelectorSettings_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBDC_LevelSelectorSettings.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UBDC_LevelSelectorSettings);
UBDC_LevelSelectorSettings::~UBDC_LevelSelectorSettings() {}
// ********** End Class UBDC_LevelSelectorSettings *************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_UE_Projects_LoH_TheTavern_Plugins_BDC_LevelSelector_Source_BDC_LevelSelector_Public_BDC_LevelSelectorSettings_h__Script_BDC_LevelSelector_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBDC_LevelSelectorSettings, UBDC_LevelSelectorSettings::StaticClass, TEXT("UBDC_LevelSelectorSettings"), &Z_Registration_Info_UClass_UBDC_LevelSelectorSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBDC_LevelSelectorSettings), 621779683U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_UE_Projects_LoH_TheTavern_Plugins_BDC_LevelSelector_Source_BDC_LevelSelector_Public_BDC_LevelSelectorSettings_h__Script_BDC_LevelSelector_2654338308(TEXT("/Script/BDC_LevelSelector"),
	Z_CompiledInDeferFile_FID_UE_Projects_LoH_TheTavern_Plugins_BDC_LevelSelector_Source_BDC_LevelSelector_Public_BDC_LevelSelectorSettings_h__Script_BDC_LevelSelector_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_UE_Projects_LoH_TheTavern_Plugins_BDC_LevelSelector_Source_BDC_LevelSelector_Public_BDC_LevelSelectorSettings_h__Script_BDC_LevelSelector_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
