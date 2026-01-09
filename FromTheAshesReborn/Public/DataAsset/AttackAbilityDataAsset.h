#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "AttackAbilityDataAsset.generated.h"

class UHitCueObject;
class UGA_ReceiveHit;
class UGameplayEffect;

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
	
};

UCLASS()
class FROMTHEASHESREBORN_API UAttackAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FAttackDataStruct AttackData;

};
