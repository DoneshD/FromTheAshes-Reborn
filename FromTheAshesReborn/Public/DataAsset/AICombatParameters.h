#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AICombatParameters.generated.h"

USTRUCT(BlueprintType)
struct FStrafeData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AcceptableRadius = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementSpeed = 150.0f;
};

UCLASS()
class FROMTHEASHESREBORN_API UAICombatParameters : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStrafeData StrafeData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AbilityStarted = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.MeleeAttack.Started");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AbilityRunning = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.MeleeAttack.Active");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AbilityFinished = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.MeleeAttack.Finished");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int TokenAmount = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AirStunFinishedTag = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.AirStunned.Finished");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag DownedFinishedTag = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.Downed.Finished");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinimumCombatRangeDistance = 900.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinimumDistanceForAttack = 250.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TestDistFloat = 250.0f;
	
};
