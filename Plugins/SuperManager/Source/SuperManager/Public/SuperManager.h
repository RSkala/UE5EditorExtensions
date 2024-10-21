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

private:

#pragma region ContentBrowserMenuExtension

	void InitContentBrowserMenuExtension();

	// Set up initial creation of our Menu Entry (First Binding)
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths); 

	// Function called in order to "build" our custom Menu Entry (Second Binding)
	void AddContentBrowserMenuEntry(class FMenuBuilder& MenuBuilder);

	// The function that the Menu Entry will call (the Menu Entry Action) (Third Binding)
	void OnDeleteUnusedAssetButtonClicked();

	// Delete empty folders
	void OnDeleteEmptyFolderseButtonClicked();

	void FixUpRedirectors();

private:
	TArray<FString> SelectedFolderPaths;

#pragma endregion // ContentBrowserMenuExtension
};
