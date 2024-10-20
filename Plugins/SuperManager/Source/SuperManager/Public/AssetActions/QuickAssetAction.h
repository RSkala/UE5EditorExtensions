// Copyright Richard Skala

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "QuickAssetAction.generated.h"

UCLASS()
class SUPERMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor)
	void TestFunc();

private:
	// NOTE:
	// * I tried Removing the private to see if these functions refresh without having to recompile the Blueprint child asset.
	// * The TestFunc() always seems to appear, but I wonder if that's due to it 
	//   being the first function created before the blueprint child class was created
	// * Update: Doesn't matter, you still have to re-compile the Blueprint.

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void DuplicateAssets(int32 NumOfDuplicates);

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void TestCallInEditorString(FString TestString);

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true)) // AllowPrivateAccess is needed for the dialog to show up when private
	void PrintViewportAspectRatio();
};
