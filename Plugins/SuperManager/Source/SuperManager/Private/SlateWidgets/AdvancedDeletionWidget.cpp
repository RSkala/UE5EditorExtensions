// Copyright Richard Skala

#include "SlateWidgets/AdvancedDeletionWidget.h"

#include "SlateBasics.h" // Needed for SScrollBox?

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
				.AutoHeight()
				[
					SNew(SScrollBox)
				]

			// Fourth slot for 3 buttons
			+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
				]
	];
}
