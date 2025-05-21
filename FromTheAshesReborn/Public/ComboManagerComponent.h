#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "ComboManagerComponent.generated.h"

struct FMeleeAttackForms;
class UMeleeAbilityDataAsset;
class UFTAAbilityDataAsset;
class UFTAAbilitySystemComponent;
class UFTAGameplayAbility;
class AFTAPlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	int32 AerialAttacksCounter = 0;

	bool PauseCurrentAttack = false;

protected:
	
	TObjectPtr<UFTAAbilitySystemComponent> FTAASC;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer CurrentComboTagContainer;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentComboIndex = 0;


	FTimerHandle AerialAttacksTimerHandle;

protected:
	
	UComboManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static void PrintGameplayTagsInContainer(const FGameplayTagContainer& Container);

public:
	
	FGameplayTagContainer& GetCurrentComboContainer();
	int GetCurrentComboIndex() const;
	void SetCurrentComboIndex(int Index);

	bool FindMatchingMeleeAssetToTagContainer(const FMeleeAttackForms& MeleeAssets, TObjectPtr<UMeleeAbilityDataAsset>& OutMatchingMeleeAsset);
	
};