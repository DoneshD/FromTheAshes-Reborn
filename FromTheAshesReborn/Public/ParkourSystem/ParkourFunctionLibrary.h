#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParkourFunctionLibrary.generated.h"

struct FGameplayTag;

UCLASS()
class FROMTHEASHESREBORN_API UParkourFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FRotator NormalReverseRotationZ(FVector NormalVector);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FRotator ReverseRotation(FRotator Rotation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float SelectClimbStyleFloat(float Braced, float FreeHang, FGameplayTag ClimbStyle);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float SelectDirectionFloat(float Forward, float Backward, float Left, float Right, float ForwardLeft,
		float ForwardRight, float BackwardLeft, float BackwardRight, FGameplayTag Direction);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float SelectParkourStateFloat(float NotBusy, float Vault, float Mantle, float Climb, FGameplayTag ParkourState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag SelectDirectionHopAction(FGameplayTag Forward, FGameplayTag Backward, FGameplayTag Left, FGameplayTag Right, FGameplayTag ForwardLeft,
		FGameplayTag ForwardRight, FGameplayTag BackwardLeft, FGameplayTag BackwardRight, FGameplayTag Direction);
};
