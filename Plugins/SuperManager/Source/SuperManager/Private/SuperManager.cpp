// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "ContentBrowserModule.h"

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
	}

	return MenuExtender;
}

void FSuperManagerModule::AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
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
	UE_LOG(LogTemp, Warning, TEXT("FSuperManagerModule::OnDeleteUnusedAssetButtonClicked"));
}

#pragma endregion // ContentBrowserMenuExtension

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)