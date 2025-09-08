#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "MeleePropertiesComponent.generated.h"

class UGameplayEffect;
class UGA_ReceiveHit;

USTRUCT(BlueprintType)
struct FMeleeAttackDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TArray<TSubclassOf<UGA_ReceiveHit>> PossibleHitReactions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FVector WeaponTraceSize;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	ESpatialDirection AttackDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TObjectPtr<UNiagaraSystem> SlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data", Meta = (Categories = "GameplayCue"))
	FGameplayTag SlashFXCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TObjectPtr<UNiagaraSystem> HitFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data", Meta = (Categories = "GameplayCue"))
	FGameplayTag HitFXCueTag;
	
	FMeleeAttackDataStruct()
	:
	WeaponTraceSize(20.0f, 20.0f, 20.0f),
	AttackDirection(ESpatialDirection::Front),
	SlashFX(nullptr),
	HitFX(nullptr)
	{}
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeleeRuntimeDataSetSignature, FMeleeAttackDataStruct, InMeleeData);

class UNiagaraSystem;
class AFTACharacter;
class UEquipmentManagerComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UMeleePropertiesComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;

public:

	UPROPERTY(BlueprintCallable, Category = "Events")
	FOnMeleeRuntimeDataSetSignature OnSetMeleeData;
	
	UMeleePropertiesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
