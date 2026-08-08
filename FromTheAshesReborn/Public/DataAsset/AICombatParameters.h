#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AICombatParameters.generated.h"

class UEnvQuery;

USTRUCT(BlueprintType)
struct FStrafeData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Strafe")
	float AcceptableRadius = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Strafe")
	float MovementSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strafe")
	TObjectPtr<UEnvQuery> StrafeQuery;
};

USTRUCT(BlueprintType)
struct FGroupCombatData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int TokenAmount = 1;
};

USTRUCT(BlueprintType)
struct FChaseData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinimumCombatRangeDistance = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Strafe")
	float AcceptableRadius = 100.0f;
};

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinimumDistanceForAttack = 250.0f;
};

UCLASS()
class FROMTHEASHESREBORN_API UAICombatParameters : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStrafeData StrafeData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGroupCombatData GroupCombatData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FChaseData ChaseData;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAttackData AttackData;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AirStunFinishedTag = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.AirStunned.Finished");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag DownedFinishedTag = FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.Downed.Finished");
	
};
