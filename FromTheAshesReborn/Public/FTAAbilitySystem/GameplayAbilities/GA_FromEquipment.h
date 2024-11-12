#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_FromEquipment.generated.h"

class UFTAEquipmentInstance;

UCLASS()
class FROMTHEASHESREBORN_API UGA_FromEquipment : public UFTAGameplayAbility
{
	GENERATED_BODY()

public:
	
	UGA_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	UFTAEquipmentInstance* GetAssociatedEquipment() const;

	
};
