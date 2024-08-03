#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem//FTAAbilitySystemGlobals.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameplayTagContainer.h"

UFTACharacterMovementComponent::UFTACharacterMovementComponent()
{
	//SprintSpeedMultiplier = 1.4f;
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
	return Owner->GetMoveSpeed();
}