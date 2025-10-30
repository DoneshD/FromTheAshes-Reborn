#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "GameplayCueNotifyTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "MeleePropertiesComponent.generated.h"

class USlashCueObject;
class UNiagaraSystem;
class UGameplayEffect;
class UGA_ReceiveHit;
class AFTACharacter;
class UEquipmentManagerComponent;

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
struct FWeaponTraceSizeStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEnabled;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector WeaponTraceSize;

	FWeaponTraceSizeStruct()
	:
	bEnabled(false),
	WeaponTraceSize(20.0f, 20.0f, 20.0f)
	{}
	
	
};

USTRUCT(BlueprintType)
struct FMeleeAttackDataStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TArray<TSubclassOf<UGA_ReceiveHit>> PossibleHitReactions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FWeaponTraceSizeStruct WeaponTraceSizeStruct;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FAttackDirectionStruct AttackDirectionStruct;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<USlashCueObject> SlashCueClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TObjectPtr<UNiagaraSystem> HitFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data", Meta = (Categories = "GameplayCue"))
	FGameplayTag HitFXCueTag;

	
	
	FMeleeAttackDataStruct()
	:
	WeaponTraceSizeStruct(),
	AttackDirectionStruct(),
	HitFX(nullptr)
	{}
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeleeRuntimeDataSetSignature, FMeleeAttackDataStruct, InMeleeData);

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
