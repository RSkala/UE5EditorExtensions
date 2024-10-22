// Copyright Richard Skala

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvancedDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeletionTab) {}

	SLATE_ARGUMENT(FString, TestString)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
};
