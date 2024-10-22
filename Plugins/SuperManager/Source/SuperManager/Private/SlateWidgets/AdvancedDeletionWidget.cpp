// Copyright Richard Skala

#include "SlateWidgets/AdvancedDeletionWidget.h"

void SAdvancedDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	// Access our arguments like this:
	//InArgs._TestString;

	ChildSlot
	[
		SNew(STextBlock)
			.Text(FText::FromString(InArgs._TestString))
	];
}
