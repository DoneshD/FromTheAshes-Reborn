#include "TimelineHelper.h"

UTimelineComponent* TimelineHelper::CreateTimeline(UTimelineComponent* Timeline, AActor* Target, UCurveFloat* LerpCurve, FName TimelineName, FName InterpFunctionName, FName FinishedFunctionName)
{
    FOnTimelineFloat InterpFunction;
    FOnTimelineEvent TimelineFinished;

    InterpFunction.BindUFunction(Target, InterpFunctionName);
    TimelineFinished.BindUFunction(Target, FinishedFunctionName);

    Timeline->AddInterpFloat(LerpCurve, InterpFunction, FName("Alpha"));
    Timeline->SetTimelinePostUpdateFunc(TimelineFinished);

    Timeline->SetLooping(false);
    Timeline->SetIgnoreTimeDilation(true);

    return Timeline;
}
