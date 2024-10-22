// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"

#include "DebugHeader.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// NOTE: Because we set this module's LoadingPhase to "PreDefault" (in the uplugin), this will load before the game module

	// Initialize our Content Browser Menu Extension
	InitContentBrowserMenuExtension();

	// Initialize our Custom Tab
	RegisterAdvancedDeletionTab();

	// Initialize test viewport tab
	//RegisterTestViewportTab();
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

// Define the location of our custom menu entry. This is called EACH time the Right-click -> Content Browser menu is opened
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
			FName("Delete"), // ExtensionHook, position to insert our menu entry. This MUST exist.
			EExtensionHook::After, // Extension position, before or after the above ExtensionHook
			TSharedPtr<FUICommandList>(), // Custom HotKeys to trigger this (here, using "empty")
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddContentBrowserMenuEntry)); // Delegate called to populate abd define the details of our the menu

		// Set the selected paths
		SelectedFolderPaths = SelectedPaths;
	}

	return MenuExtender;
}

// Defining the details of our custom menu entry
void FSuperManagerModule::AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::AddContentBrowserMenuEntry"));

	// Delete Unused Assets
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")), // Label / Title text for our menu entry
		FText::FromString(TEXT("Safely delete all unused assets under folder")), // Tooltip
		FSlateIcon(), // Custom Icon
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonClicked) // Actual function to call when our custom Menu Entry is selected
	);

	// Delete Empty Folders
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Safely delete any empty folders under the selected folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFolderseButtonClicked)
	);

	// Advanced asset deletion
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Advanced Deletion")),
		FText::FromString(TEXT("List assets by specific condition in a tab for deleting")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnAdvancedDeletionButtonClicked)
	);

	// Test viewport tab
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Test Viewport Window")),
		FText::FromString(TEXT("Create a new tab window with a viewport inside")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnTestViewportTabButtonClicked)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	//UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::OnDeleteUnusedAssetButtonClicked"));
	DebugHeader::Print(TEXT("OnDeleteUnusedAssetButtonClicked"), FColor::Green);

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

	// Get ALL the assets under the selected folder
	TArray<FString> AssetsPathsNames = UEditorAssetLibrary::ListAssets(SelectedFolderPaths[0], true, false);

	// Exit if there are no assets under the selected folder
	if (AssetsPathsNames.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No assets found under selected folder"), false);
		return;
	}

	// Show confirmation dialog to the user in order to proceed
	EAppReturnType::Type ConfirmResult = 
		DebugHeader::ShowMsgDialog(
			EAppMsgType::YesNo,
			TEXT("A total of ") + FString::FromInt(AssetsPathsNames.Num()) + TEXT(" assets need to be checked.\n\nWould you like to proceed?"), false);

	// Exit if the user does not wish to proceed (i.e. selected "No" in the dialog box)
	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}

	// Fix up Redirectors BEFORE deleting unused assets
	FixUpRedirectors();

	// List of assets to be deleted
	TArray<FAssetData> UnusedAssetsDataArray;

	// Iterate through the assets found under the selected folder and add to the "unused assets" list
	for (const FString& AssetPathName : AssetsPathsNames)
	{
		// Don't touch root folder. Do not delete anything from Collections or Developers folders!
		if (AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		// Ensure the asset exists (possibly removed when fixing up redirectors?)
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		// Get all referencers to the current asset
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencers.Num() == 0)
		{
			// There are NO referencers to the current asset. Add to the list of assets to be deleted.
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	// If there are any elements in the "unused assets" array, delete them
	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused assets found under selected folder"), false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFolderseButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("---------------------------------------------"));
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::OnDeleteEmptyFolderseButtonClicked"));

	if (SelectedFolderPaths.Num() != 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}

	// Fix up redirectors before attempting to delete folders (as a folder could be empty after deleting redirectors)
	FixUpRedirectors();

	FString EmptyFolderPathsNames; // for debug messages
	uint32 Counter = 0;

	// Get all assets INCLUDING paths (last parameter is bIncludeFolder)
	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(SelectedFolderPaths[0], true, true);

	// List of empty paths to delete
	TArray<FString> EmptyFoldersPathsArray;

	// Iterate through each found asset, check if it is a directory, then check if empty
	for (const FString& FolderPath : FolderPathsArray)
	{
		// Don't touch root folder. Do not delete anything from Collections or Developers folders!
		if (FolderPath.Contains(TEXT("Collections")) ||
			FolderPath.Contains(TEXT("Developers")) ||
			FolderPath.Contains(TEXT("__ExternalActors__")) ||
			FolderPath.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		// If the directory does NOT exist, then it is an asset, not a directory. Skip.
		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
		{
			continue;
		}

		// Check if directory is empty
		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append(TEXT("\n"));

			EmptyFoldersPathsArray.Add(FolderPath);
		}
	}

	// Did we find any empty folders
	if (EmptyFoldersPathsArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No empty folders found under selected folder"), false);
		return;
	}
	
	// Show user confirmation
	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMsgDialog(
		EAppMsgType::OkCancel,
		TEXT("Empty folders found in:\n") + EmptyFolderPathsNames + TEXT("\nWould you like to delete all?"),
		false);

	if (ConfirmResult == EAppReturnType::Cancel)
	{
		return;
	}

	for (const FString& EmptyFolderPath : EmptyFoldersPathsArray)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath))
		{
			++Counter;
		}
		else
		{
			DebugHeader::Print(TEXT("Failed to delete " + EmptyFolderPath), FColor::Red);
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(Counter) + TEXT(" folders."));
	}

	// ===============================================

	// Get the Asset Registry in order to access folders
	//IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();

	// Get all the paths in the content browser / asset registry
	// NOTE: // This gets ALL paths in the project, including ones under Engine, etc!
	/*TArray<FString> CachedPaths;
	//AssetRegistry.GetAllCachedPaths(CachedPaths); 
	UE_LOG(LogTemp, Log, TEXT("Cached Paths:"));
	for (FString CachedPath : CachedPaths)
	{
		UE_LOG(LogTemp, Log, TEXT(" - %s"), *CachedPath);
	}*/

	// Get all subpaths to the selected path (recursively)
	//TArray<FString> SubPaths;
	//AssetRegistry.GetSubPaths(SelectedFolderPaths[0], SubPaths, true);

	//ObjectTools
}

void FSuperManagerModule::OnAdvancedDeletionButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::OnDeleteEmptyFolderseButtonClicked"));

	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedDeletion"));
}

void FSuperManagerModule::OnTestViewportTabButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("TestViewportTab"));
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

#pragma region CustomEditorTab

void FSuperManagerModule::RegisterAdvancedDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner
	(
		FName("AdvancedDeletion"),
		FOnSpawnTab::CreateRaw(
			this,
			&FSuperManagerModule::OnSpawnAdvancedDeletionTab),
		FCanSpawnTab::CreateRaw(
			this,
			&FSuperManagerModule::CanSpawnAdvancedDeletionTab)
	).SetDisplayName(FText::FromString(TEXT("Advanced Deletion")));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
		SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvancedDeletionTab)
				.TestString(TEXT("I am passing data"))
		];


	//return TSharedRef<SDockTab>();
}

bool FSuperManagerModule::CanSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return true;
}

#pragma endregion // CustomEditorTab

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma region TestViewportTab

void FSuperManagerModule::RegisterTestViewportTab()
{
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::RegisterTestViewportTab"));
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner
	(
		FName("TestViewportTab"),
		FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnTestViewportTab),
		FCanSpawnTab::CreateRaw(this, &FSuperManagerModule::CanSpawnTestViewportTab)
	)
	.SetDisplayName(FText::FromString(TEXT("Test Viewport")));
	//.SetMenuType(ETabSpawnerMenuType::Enabled);
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnTestViewportTab(const FSpawnTabArgs& SpawnTabArgs)
{
	/*return
		SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvancedDeletionTab)
				.TestString(TEXT("I am passing data"))
		];*/

	/*return SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[

	];*/

	//return SNew(SDockTab)
	//[
	//	SNew(SViewport)
	//		.EnableGammaCorrection(false)
	//		.IgnoreTextureAlpha(false)
	//		// Link it with your SceneViewport here (details below)
	//];

	/*TSharedPtr<SViewport> ViewportWidget = SNew(SViewport)
		.EnableGammaCorrection(false);

	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SViewport)
			.EnableGammaCorrection(false)
			.IgnoreTextureAlpha(false)
	];*/

	//auto ViewportClient = MakeShareable(new FCommonViewportClient(SharedThis(this)));

	//ViewportClient = MakeShareable(new FCommonViewportClient());

	//TSharedPtr<FSceneViewport> SceneViewport = MakeShareable(new FSceneViewport(ViewportClient.Get(), ViewportWidget));
	//ViewportWidget->SetViewportInterface(SceneViewport.ToSharedRef());

	


	//return SNew(SDockTab).TabRole(ETabRole::NomadTab); // The bare minimum for a tab window
	return SNew(SDockTab); // The bare minimum for a tab window

	//return TSharedRef<SDockTab>(); // Leaving this as-is causes a build error.
}

bool FSuperManagerModule::CanSpawnTestViewportTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return true;
}

#pragma endregion // TestViewportTab

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)