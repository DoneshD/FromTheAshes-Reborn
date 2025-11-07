#include "CombatComponents/CentralStateComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCentralStateComponent::UCentralStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCentralStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

	if(!ASC)
	{
		UE_LOG(LogTemp, Error , TEXT("ASC not found on [%s]"), *GetOwner()->GetActorNameOrLabel());
		return;
	}
	
	if(!ASC->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error , TEXT("Invalid ASC found on [%s]"), *GetOwner()->GetActorNameOrLabel())
		return;
	}

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if(!OwnerChar)
	{
		UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::BeginPlay() - OwnerChar is invalid"))
		return;
	}

	CMC = OwnerChar->GetCharacterMovement();

	if(!CMC)
	{
		UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::BeginPlay() - CMC is invalid"))
		return;
	}
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

void UCentralStateComponent::SetCurrentOrientation(FGameplayTag OrientationTag, EMovementMode MovementMode)
{
	if(CurrentStateTag.MatchesTagExact(OrientationTag))
	{
		// UE_LOG(LogTemp, Error, TEXT("UCentralStateComponent::SetCurrentOrientation - Already set [%s] to [%s]"),
		// 	*GetOwner()->GetActorNameOrLabel(), *OrientationTag.GetTagName().ToString())
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
			HandeAirborneOrientation(MovementMode);
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
	if(!ASC->HasMatchingGameplayTag(NeutralStateTag))
	{
		ASC->AddLooseGameplayTag(NeutralStateTag);
	}
	if(ASC->HasMatchingGameplayTag(DownedStateTag))
	{
		ASC->RemoveLooseGameplayTag(DownedStateTag);
	}
	CurrentStateTag = NeutralStateTag;
}

void UCentralStateComponent::HandleDownedState()
{
	if(!ASC->HasMatchingGameplayTag(DownedStateTag))
	{
		ASC->AddLooseGameplayTag(DownedStateTag);
	}
	if(ASC->HasMatchingGameplayTag(NeutralStateTag))
	{
		ASC->RemoveLooseGameplayTag(NeutralStateTag);
	}
	CurrentStateTag = DownedStateTag;
}

void UCentralStateComponent::HandleGroundedOrientation()
{
	if(!ASC->HasMatchingGameplayTag(GroundedOrientationTag))
	{
		ASC->AddLooseGameplayTag(GroundedOrientationTag);
	}
	if(ASC->HasMatchingGameplayTag(AirborneOrientationTag))
	{
		ASC->RemoveLooseGameplayTag(AirborneOrientationTag);
	}
	CurrentStateTag = GroundedOrientationTag;
	CMC->SetMovementMode(MOVE_Walking);
	
	AFTACharacter* FTAChar = Cast<AFTACharacter>(GetOwner());

	if(FTAChar)
	{
		FTAChar->RemoveAerialEffects();
	}
}

void UCentralStateComponent::HandeAirborneOrientation(EMovementMode MovementMode)
{
	if(!ASC->HasMatchingGameplayTag(AirborneOrientationTag))
	{
		ASC->AddLooseGameplayTag(AirborneOrientationTag);
	}
	if(ASC->HasMatchingGameplayTag(GroundedOrientationTag))
	{
		ASC->RemoveLooseGameplayTag(GroundedOrientationTag);
	}
	if(ASC->HasMatchingGameplayTag(DownedStateTag))
	{
		ASC->RemoveLooseGameplayTag(DownedStateTag);
	}
	
	CurrentStateTag = AirborneOrientationTag;

	if(MovementMode == MOVE_Falling)
	{
		CMC->SetMovementMode(MOVE_Falling);
	}
	else if(MovementMode == MOVE_Flying)
	{
		CMC->SetMovementMode(MOVE_Flying);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCentralStateComponent:: HandeAirborneOrientation - Invalid movement mode"))
	}
	
}

