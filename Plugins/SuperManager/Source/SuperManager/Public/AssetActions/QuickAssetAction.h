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
	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void DuplicateAssets(int32 NumOfDuplicates);

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void TestCallInEditorString(FString TestString);

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true)) // AllowPrivateAccess is needed for the dialog to show up when private
	void PrintViewportAspectRatio();
	
};
