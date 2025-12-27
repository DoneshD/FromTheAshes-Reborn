#pragma once

#include "Widgets/SCompoundWidget.h"

class BaseLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(BaseLoadingScreen) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FSlateFontInfo Font;
};

