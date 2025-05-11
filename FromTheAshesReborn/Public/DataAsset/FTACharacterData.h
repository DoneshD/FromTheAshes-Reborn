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
	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA|Pawn")
	TSubclassOf<ACharacter> CharacterClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA|Abilities")
	TArray<TObjectPtr<UFTAAbilitySet>> CharacterAbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA|Abilities")
	TObjectPtr<UFTAAbilityTagRelationshipMapping> CharacterTagRelationshipMapping;
};
