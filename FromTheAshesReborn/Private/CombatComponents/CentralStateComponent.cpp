#include "CombatComponents/CentralStateComponent.h"

UCentralStateComponent::UCentralStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCentralStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCentralStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FGameplayTag UCentralStateComponent::GetCurrentState()
{
	return CurrentStateTag;
}

FGameplayTag UCentralStateComponent::GetCurrentOrientation()
{
	return CurrentOrientationTag;
}

void UCentralStateComponent::SetCurrentState(FGameplayTag StateTag)
{
	if(CurrentStateTag.MatchesTagExact(StateTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::SetCurrentState - Already set [%s] to [%s]"),
			*GetOwner()->GetActorNameOrLabel(), *StateTag.GetTagName().ToString())
		return;
	}
	if(!CurrentStateTag.MatchesTagExact(StateTag))
	{
		if(StateTag.MatchesTagExact(NeutralStateTag))
		{
			HandleNeutralState();
		}
		else if(StateTag.MatchesTagExact(DownedStateTag))
		{
			HandleDownedState();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::SetCurrentState - [%s] - [%s] is does not match any state"),
				*GetOwner()->GetActorNameOrLabel(), *StateTag.GetTagName().ToString())
		}
	}
}

void UCentralStateComponent::SetCurrentOrientation(FGameplayTag OrientationTag)
{
	if(CurrentStateTag.MatchesTagExact(OrientationTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::SetCurrentOrientation - Already set [%s] to [%s]"),
			*GetOwner()->GetActorNameOrLabel(), *OrientationTag.GetTagName().ToString())
		return;
	}
	if(!CurrentStateTag.MatchesTagExact(OrientationTag))
	{
		if(OrientationTag.MatchesTagExact(GroundedOrientationTag))
		{
			HandleGroundedOrientation();
		}
		else if(OrientationTag.MatchesTagExact(AirborneOrientationTag))
		{
			HandeAirborneOrientation();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::SetCurrentState - [%s] - [%s] is does not match any orientation"),
				*GetOwner()->GetActorNameOrLabel(), *OrientationTag.GetTagName().ToString())
		}
	}
}

void UCentralStateComponent::HandleNeutralState()
{
	
}

void UCentralStateComponent::HandleDownedState()
{
	
}

void UCentralStateComponent::HandleGroundedOrientation()
{
	
}

void UCentralStateComponent::HandeAirborneOrientation()
{
	
}

