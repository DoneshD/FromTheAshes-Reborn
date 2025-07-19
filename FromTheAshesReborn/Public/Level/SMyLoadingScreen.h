#pragma once

#include "Widgets/SCompoundWidget.h"

class SMyLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyLoadingScreen) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FSlateFontInfo Font;
};
