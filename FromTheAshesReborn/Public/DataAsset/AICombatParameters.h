#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AICombatParameters.generated.h"

class UGA_Attack;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UGA_Attack> PossibleAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AssociatedEventTag = FGameplayTag::EmptyTag;
	
};

USTRUCT(BlueprintType)
struct FRoleStats
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InitialWeight = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseWeight = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FinalWeight = 0.0f;
};


UCLASS()
class FROMTHEASHESREBORN_API UAICombatParameters : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actions")
	FAttackData AttackData;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actions")
	FStrafeData StrafeData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actions")
	FChaseData ChaseData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Role")
	FRoleStats AggressionStats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Role")
	FRoleStats CoverStats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Role")
	FRoleStats ObserverStats;

};
