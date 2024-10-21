// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "DebugHeader.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// NOTE: Because we set this module's LoadingPhase to "PreDefault" (in the uplugin), this will load before the game module

	// Initialize our Content Browser Menu Extension
	InitContentBrowserMenuExtension();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitContentBrowserMenuExtension()
{
	// Get the ContentBrowserModule so we can add our own custom context menus
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	// Get the delegates for extending content browser menus
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserMenuExtenders = 
		ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	/*
	// Add our own content browser menu delegate
	FContentBrowserMenuExtender_SelectedPaths CustomContentBrowserMenuDelegate;

	// Bind our delegate to a function with the appropriate paramters
	CustomContentBrowserMenuDelegate.BindRaw(this, &FSuperManagerModule::CustomContentBrowserMenuExtender);

	// Add our delegate to the list of all "Context Menu Extenders"
	ContentBrowserMenuExtenders.Add(CustomContentBrowserMenuDelegate);
	*/

	// We can do the above using one line for simplicity (i.e. use CreateRaw() instead of doing BindRaw() then Add()
	ContentBrowserMenuExtenders.Add(
		FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomContentBrowserMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths)
{
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::CustomContentBrowserMenuExtender - NumSelectedPaths: %d"), SelectedPaths.Num());

	// Reminder: TSharedRef CANNOT point to null (must be a valid object)

	TSharedRef<FExtender>MenuExtender(new FExtender()); // When using new() in UE, be sure to always use SmartPointers to avoid memory leaks

	// The "SelectedPaths" is all the folders that are currently selected by the user 
	if (SelectedPaths.Num() > 0)
	{
		// We need an extension hook, which is the position where we want to insert our menu entry
		// To quickly find all the extension hooks:
		// * Editor Preferences -> General -> Miscellaneous -> Developer Tools
		//   * Display UI Extension Points: ENABLE

		MenuExtender->AddMenuExtension(
			FName("Delete"), // ExtensionHook
			EExtensionHook::After, // Extension position
			TSharedPtr<FUICommandList>(), // Custom HotKeys to trigger this (here, using "empty")
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddContentBrowserMenuEntry)); // Delegate called to populate the menu

		// Set the selected paths
		SelectedFolderPaths = SelectedPaths;
	}

	return MenuExtender;
}

void FSuperManagerModule::AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::AddContentBrowserMenuEntry"));

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")), // Label / Title
		FText::FromString(TEXT("Safely delete all unused assets under folder")), // Tooltip
		FSlateIcon(), // Custom Icon
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonClicked) // Actual function to call when our custom Menu Entry is selected
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	//UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::OnDeleteUnusedAssetButtonClicked"));
	DebugHeader::Print(TEXT("OnDeleteUnusedAssetButtonClicked"), FColor::Green);

	// ListAssets

	if (SelectedFolderPaths.Num() == 0)
	{
		return;
	}

	if (SelectedFolderPaths.Num() > 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}

	DebugHeader::Print(TEXT("Currently selected folder: ") + SelectedFolderPaths[0], FColor::Green);

	TArray<FString> AssetsPathsNames = UEditorAssetLibrary::ListAssets(SelectedFolderPaths[0]);
	if (AssetsPathsNames.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No assets found under selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmResult = 
		DebugHeader::ShowMsgDialog(
			EAppMsgType::YesNo,
			TEXT("A total of ") + FString::FromInt(AssetsPathsNames.Num()) + TEXT(" assets found.\n\nWould you like to proceed?"));

	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}

	// Fix up Redirectors BEFORE deleting unused assets
	FixUpRedirectors();

	// Iterate through the found assets
	TArray<FAssetData> UnusedAssetsDataArray;
	for (const FString& AssetPathName : AssetsPathsNames)
	{
		// Don't touch root folder. Do not delete anything from Collections or Develops folders!
		if (AssetPathName.Contains(TEXT("Collections")) || AssetPathName.Contains(TEXT("Collections")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused assets found under selected folder"));
	}
}

void FSuperManagerModule::FixUpRedirectors()
{
	// COPY PASTED (with some slight modifications):
	// From UQuickAssetAction::FixUpRedirectors()

	// Get the AssetRegistry in order to delete redirectors
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();

	// Create an Asset Registry Filter from the paths
	FARFilter ARFilter;
	ARFilter.bRecursivePaths = true;
	ARFilter.PackagePaths.Emplace("/Game");
	ARFilter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	// Query for a list of assets in the selected paths
	TArray<FAssetData> RedirectorAssetList;
	AssetRegistry.GetAssets(ARFilter, RedirectorAssetList);

	// Exit if no Redirector assets were found
	if (RedirectorAssetList.Num() == 0)
	{
		return;
	}

	// Get all the Redirector asset paths
	TArray<FString> RedirectorObjectPaths;
	for (const FAssetData& RedirectorAsset : RedirectorAssetList)
	{
		RedirectorObjectPaths.Add(RedirectorAsset.GetObjectPathString());
	}

	// Create LoadAssetsSettings 
	AssetViewUtils::FLoadAssetsSettings LoadAssetsSettings;
	LoadAssetsSettings.bFollowRedirectors = false;
	LoadAssetsSettings.bAllowCancel = true;

	// Array of Redirector objects that will be loaded in order for fixup
	TArray<UObject*> RedirectorObjects;

	// Load Redirector objects for fixup
	AssetViewUtils::ELoadAssetsResult LoadAssetsResult = AssetViewUtils::LoadAssetsIfNeeded(RedirectorObjectPaths, RedirectorObjects, LoadAssetsSettings);

	// If the user confirmed fixup, then apply the fixup redirector action
	if (LoadAssetsResult != AssetViewUtils::ELoadAssetsResult::Cancelled)
	{
		// Transform Objects array to ObjectRedirectors array
		TArray<UObjectRedirector*> RedirectorsToFix;
		for (UObject* RedirectorObject : RedirectorObjects)
		{
			RedirectorsToFix.Add(CastChecked<UObjectRedirector>(RedirectorObject));
		}

		// Load the asset tools module (used for fixing up the Redirectors)
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
	}
}

#pragma endregion // ContentBrowserMenuExtension

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)