#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem//FTAAbilitySystemGlobals.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameplayTagContainer.h"

UFTACharacterMovementComponent::UFTACharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
	ADSSpeedMultiplier = 0.8f;
	KnockedDownSpeedMultiplier = 0.4f;

	KnockedDownTag = FGameplayTag::RequestGameplayTag("State.KnockedDown");
	InteractingTag = FGameplayTag::RequestGameplayTag("State.Interacting");
	InteractingRemovalTag = FGameplayTag::RequestGameplayTag("State.InteractingRemoval");
}

float UFTACharacterMovementComponent::GetMaxSpeed() const
{
	AFTACharacter* Owner = Cast<AFTACharacter>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	// Don't move while interacting or being interacted on (revived)
	if (Owner->GetAbilitySystemComponent() && Owner->GetAbilitySystemComponent()->GetTagCount(InteractingTag)
		> Owner->GetAbilitySystemComponent()->GetTagCount(InteractingRemovalTag))
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent() && Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(KnockedDownTag))
	{
		return Owner->GetMoveSpeed() * KnockedDownSpeedMultiplier;
	}

	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
}



void UFTACharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UFTACharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

