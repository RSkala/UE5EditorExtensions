// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#pragma region ContentBrowserMenuExtension

private:
	void InitContentBrowserMenuExtension();

	// Set up initial creation of our Menu Entry (First Binding)
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths); 

	// Function called in order to "build" our custom Menu Entry (Second Binding)
	void AddContentBrowserMenuEntry(class FMenuBuilder& MenuBuilder);

	// The function that the Menu Entry will call (the Menu Entry Action) (Third Binding)
	void OnDeleteUnusedAssetButtonClicked();

	// Delete empty folders
	void OnDeleteEmptyFolderseButtonClicked();

	// Open Advanced Deletion Tab
	void OnAdvancedDeletionButtonClicked();

	// Open Test Viewport Tab
	void OnTestViewportTabButtonClicked();

	void FixUpRedirectors();

private:
	TArray<FString> SelectedFolderPaths;

#pragma endregion // ContentBrowserMenuExtension

#pragma region CustomEditorTab

private:

	void RegisterAdvancedDeletionTab();
	TSharedRef<SDockTab> OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs); // DECLARE_DELEGATE_RetVal_OneParam( TSharedRef<SDockTab>, FOnSpawnTab, const FSpawnTabArgs& );
	bool CanSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs); // DECLARE_DELEGATE_RetVal_OneParam(bool, FCanSpawnTab, const FSpawnTabArgs&);

#pragma endregion // CustomEditorTab

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Testing Viewports in a Tab window
#pragma region TestViewportTab

private:

	void RegisterTestViewportTab();
	TSharedRef<SDockTab> OnSpawnTestViewportTab(const FSpawnTabArgs& SpawnTabArgs);
	bool CanSpawnTestViewportTab(const FSpawnTabArgs& SpawnTabArgs);

private:
	//TSharedPtr<struct FCommonViewportClient> ViewportClient;

#pragma endregion // TestViewportTab
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
};
