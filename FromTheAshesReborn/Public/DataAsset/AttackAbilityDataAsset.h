#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "AttackAbilityDataAsset.generated.h"

class UHitCueObject;
class UGA_ReceiveHit;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAttackDirectionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEnabled;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ESpatialDirection AttackDirection;

	FAttackDirectionStruct()
	:
	bEnabled(false),
	AttackDirection(ESpatialDirection::None)
	{}
	
	
};

USTRUCT(BlueprintType)
struct FAttackDataStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TArray<TSubclassOf<UGA_ReceiveHit>> PossibleHitReactions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<UHitCueObject> HitCueClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FAttackDirectionStruct AttackDirectionStruct;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UAttackAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FAttackDataStruct AttackData;

};
