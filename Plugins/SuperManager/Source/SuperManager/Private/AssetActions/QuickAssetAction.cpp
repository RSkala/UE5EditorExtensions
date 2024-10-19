// Copyright Richard Skala

#include "AssetActions/QuickAssetAction.h"

void UQuickAssetAction::TestFunc()
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Cyan, TEXT("UQuickAssetAction::TestFunc"));
	}
}
