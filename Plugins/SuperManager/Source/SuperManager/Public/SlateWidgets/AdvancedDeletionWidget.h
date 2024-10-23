// Copyright Richard Skala

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvancedDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeletionTab) {}

	SLATE_ARGUMENT(FString, TestString)
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataArray)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TArray<TSharedPtr<FAssetData>> AssetsDataUnderSelectedFolderArray;
};
