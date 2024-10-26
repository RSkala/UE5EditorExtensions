// Copyright Richard Skala

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvancedDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeletionTab) {}

	SLATE_ARGUMENT(FString, TestString)
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	// --- Row ----
	TSharedRef<ITableRow> OnGenerateRowForList( // Function callback for OnGenerateRow(). Called when constructing a row in our listview
		TSharedPtr<FAssetData> AssetDataToDisplay, // Data for a row
		const TSharedRef<STableViewBase>& OwnerTable); // Widget that owns this row

	// --- CheckBox ---
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	// --- Asset Name and Asset Class Name Text ---
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);

	FSlateFontInfo GetEmbossedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }

private:
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;
};
