#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DownedCombatComponent.generated.h"


class UGameplayEffect;
class UGA_Recover;
class UCharacterMovementComponent;
class UFTAAbilitySystemComponent;
class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UDownedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> EnableDownedCombatEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TArray<TSubclassOf<UGA_Recover>> PossibleRecoveries;
	
	float TotalDownedTime;

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

	void DisableComponent();
};
