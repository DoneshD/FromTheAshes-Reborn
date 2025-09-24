#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DownedCombatComponent.generated.h"


class UCharacterMovementComponent;
class UFTAAbilitySystemComponent;
class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UDownedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	bool IsComponentActive = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag EnableTag;

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter;

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;
	
protected:
	
	UDownedCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EnableComponent(const FGameplayTag InEnableTag, int32 NewCount);
};
