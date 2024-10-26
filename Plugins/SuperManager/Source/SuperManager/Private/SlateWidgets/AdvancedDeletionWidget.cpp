// Copyright Richard Skala

#include "SlateWidgets/AdvancedDeletionWidget.h"

#include "SlateBasics.h" // Needed for SScrollBox?

#include "DebugHeader.h"

void SAdvancedDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	// Access our arguments like this:
	//InArgs._TestString;

	StoredAssetsData = InArgs._AssetsDataToStore;

	/*ChildSlot
	[
		SNew(STextBlock)
			.Text(FText::FromString(InArgs._TestString))
	];*/

	// Font for Title Text
	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	// Add our elements to this widget's Slot
	ChildSlot
	[
		// Main vertical box
		SNew(SVerticalBox)

			// First vertical slot for title text
			+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("Advanced Deletion")))
						.Font(TitleTextFont)
						.Justification(ETextJustify::Center)
						.ColorAndOpacity(FColor::White)
				]

			// Second Slot for drop down to specify the listing condition
			+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
				]

			// Third Slot for the asset list
			+ SVerticalBox::Slot()
				//.AutoHeight()
				.VAlign(VAlign_Fill)
				[
					SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SNew(SListView<TSharedPtr<FAssetData>>)
								.ItemHeight(24.0f)
								.ListItemsSource(&StoredAssetsData)
								.OnGenerateRow(this, &SAdvancedDeletionTab::OnGenerateRowForList)
						]
				]

			// Fourth slot for 3 buttons
			+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
				]
	];
}

TSharedRef<ITableRow> SAdvancedDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay->IsValid())
	{
		// The return type is TSharedRef which must point to a valid object, so return an empty row widget
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}

	//const FString DisplayAssetClassName = AssetDataToDisplay->AssetClass.ToString(); // Results in "None" in UE5.1+
	//const FString Dsia = AssetDataToDisplay->GetClass()->GetFName().ToString(); // Same as below
	const FString DisplayAssetClassName = AssetDataToDisplay->GetClass()->GetName();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	FSlateFontInfo AssetClassNameFont = GetEmbossedTextFont();
	AssetClassNameFont.Size = 10;

	FSlateFontInfo AssetNameFont = GetEmbossedTextFont();
	AssetNameFont.Size = 15;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
			.Padding(FMargin(5.0f)) // Add a gap between each row
		[
			SNew(SHorizontalBox)
				// First slot for check box
				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left) // Align to the left to ensure there is no gap
					.VAlign(VAlign_Center)
					.FillWidth(0.04f) // Add small gap. If this field is not set, there will be a large gap
				[
					ConstructCheckBox(AssetDataToDisplay)
				]

				// Second slot for displaying asset class name
				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					.FillWidth(0.5f)
				[
					ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
				]

				// Third slot for displaying asset name
				+SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Fill)
				[
					ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
				]

				// Fourth slot for a button
				+SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Fill)
				[
					ConstructButtonForRowWidget(AssetDataToDisplay)
				]
		];
	
	return ListViewRowWidget;

	//	return TSharedRef<ITableRow>();
}

TSharedRef<SCheckBox> SAdvancedDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox =
		SNew(SCheckBox)
			.Type(ESlateCheckBoxType::CheckBox)
			.OnCheckStateChanged(this, &SAdvancedDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
			.Visibility(EVisibility::Visible);

	// Experiment with ToggleButton
	TSharedRef<SCheckBox> ConstructedCheckBox2 =
		SNew(SCheckBox)
			.Type(ESlateCheckBoxType::ToggleButton)
			.OnCheckStateChanged(this, &SAdvancedDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
			.Visibility(EVisibility::Visible);

	return ConstructedCheckBox;
}

void SAdvancedDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnCheckBoxStateChanged - %s - NewState: %d"), *AssetData->AssetName.ToString(), NewState);

	switch (NewState)
	{
		case ECheckBoxState::Unchecked:
			DebugHeader::Print(AssetData->AssetName.ToString() + TEXT(" is UNchecked"), FColor::Red);
			break;

		case ECheckBoxState::Checked:
			DebugHeader::Print(AssetData->AssetName.ToString() + TEXT(" is CHECKED"), FColor::Green);
			break;

		case ECheckBoxState::Undetermined:
			break;

		default:
			break;
	}
}

TSharedRef<STextBlock> SAdvancedDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructedTextBlock =
		SNew(STextBlock)
			.Text(FText::FromString(TextContent))
			.Font(FontToUse)
			.ColorAndOpacity(FColor::White);

	return ConstructedTextBlock;

	//return SNew(STextBlock)
	//	.Text(FText::FromString(TextContent))
	//	.Font(FontToUse)
	//	.ColorAndOpacity(FColor::White);
}

TSharedRef<SButton> SAdvancedDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedButton =
		SNew(SButton)
			.Text(FText::FromString(TEXT("Delete")))
			.OnClicked(this, &SAdvancedDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);

	return ConstructedButton;
	//return TSharedRef<SButton>();
}

FReply SAdvancedDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	DebugHeader::Print(ClickedAssetData->AssetName.ToString() + TEXT(" is clicked"), FColor::Green);
	return FReply::Handled(); // You must return FReply::Handled, otherwise you might get a crash
}
