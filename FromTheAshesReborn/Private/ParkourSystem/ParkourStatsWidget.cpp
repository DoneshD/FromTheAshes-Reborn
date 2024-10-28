#include "ParkourSystem/ParkourStatsWidget.h"

bool UParkourStatsWidget::SetParkourState(FText ParkourState)
{
	ParkourStateTexts = ParkourState;
	return false;
}

bool UParkourStatsWidget::SetParkourAction(FText ParkourAction)
{
	ParkourActionText = ParkourAction;
	return false;
}

bool UParkourStatsWidget::SetClimbStyle(FText ClimbStyle)
{
	ClimbStyleText = ClimbStyle;
	return false;
}
