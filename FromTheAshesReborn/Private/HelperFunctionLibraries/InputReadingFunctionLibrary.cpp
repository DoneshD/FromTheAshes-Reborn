#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

FVector UInputReadingFunctionLibrary::CheckInputVector(UCharacterMovementComponent* CMC, float  ErrorMargin)
{
	if(!CMC->GetLastInputVector().IsNearlyZero(ErrorMargin))
	{
		return CMC->GetLastInputVector().GetSafeNormal();
	}
	return CMC->GetOwner()->GetActorForwardVector();
}
