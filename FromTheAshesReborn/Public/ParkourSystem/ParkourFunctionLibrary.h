
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParkourFunctionLibrary.generated.h"

struct FGameplayTag;

UCLASS()
class FROMTHEASHESREBORN_API UParkourFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:


public:

	UFUNCTION()
	FRotator NormalReverseRotationZ(FVector NormalVector);

	UFUNCTION()
	FRotator ReverseRotation(FRotator Rotation);

	UFUNCTION()
	float SelectClimbStyleFloat(float Braced, float FreeHang, FGameplayTag ClimbStyle);

	UFUNCTION()
	float SelectDirectionFloat(float Forward, float Backward, float Left, float Right, float ForwardLeft,
		float ForwardRight, float BackwardLeft, float BackwardRight, FGameplayTag Direction);

	UFUNCTION()
	float SelectParkourStateFloat(float NotBusy, float Vault, float Mantle, float Climb, FGameplayTag ParkourState);

	UFUNCTION()
	FGameplayTag SelectDirectionHopAction(FGameplayTag Forward, FGameplayTag Backward, FGameplayTag Left, FGameplayTag Right, FGameplayTag ForwardLeft,
		FGameplayTag ForwardRight, FGameplayTag BackwardLeft, FGameplayTag BackwardRight, FGameplayTag Direction);

};
