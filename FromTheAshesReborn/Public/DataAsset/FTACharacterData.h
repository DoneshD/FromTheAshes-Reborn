#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FTACharacterData.generated.h"

class UFTAAbilitySet;
class UFTAAbilityTagRelationshipMapping;

UCLASS(BlueprintType, Const, Meta = (DisplayName = "FTA Character Data"))
class FROMTHEASHESREBORN_API UFTACharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFTACharacterData(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA Character Data")
	TSubclassOf<ACharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA Character Data")
	TArray<TObjectPtr<UFTAAbilitySet>> CharacterAbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA Character Data")
	TObjectPtr<UFTAAbilityTagRelationshipMapping> CharacterTagRelationshipMapping;
};
