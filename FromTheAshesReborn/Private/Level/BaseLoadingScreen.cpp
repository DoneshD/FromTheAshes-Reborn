#include "Level/BaseLoadingScreen.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void BaseLoadingScreen::Construct(const FArguments& InArgs)
{
	Font = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 48);
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor::Black)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Loading...")))
					.Font(Font)
					.ColorAndOpacity(FLinearColor::White)
					.Justification(ETextJustify::Center)
				]
			]
		]
	];

	UE_LOG(LogTemp, Warning, TEXT("SMyLoadingScreen::Construct() - Widget Initialized"));
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

