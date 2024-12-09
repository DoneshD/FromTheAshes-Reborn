
#pragma once

#include "CoreMinimal.h"
#include "GA_FromEquipment.h"
#include "GA_MeleeWeaponAttack.generated.h"


class AWeaponActorBase;
class UMeleeWeaponInstance;
class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UFTAAT_PlayMontageAndWaitForEvent> Task;

	TObjectPtr<AWeaponActorBase> MeleeWeaponActor;

public:

	UGA_MeleeWeaponAttack(const FObjectInitializer& = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	UMeleeWeaponInstance* GetMeleeWeaponInstance() const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface


protected:
	struct FMeleeWeaponTraceData
	{
		// Start of the trace
		FVector StartTrace;

		// End of the trace if aim were perfect
		FVector EndTrace;

		// The weapon instance / source of weapon data
		UMeleeWeaponInstance* WeaponData = nullptr;

		FMeleeWeaponTraceData()
			: StartTrace(ForceInitToZero)
			, EndTrace(ForceInitToZero)
		{
		}
	};
	
	UFUNCTION(BlueprintCallable)
	void StartMeleeWeaponTargeting();

	UFUNCTION(BlueprintCallable)
	void EndMeleeWeaponTargeting();

	// Called when target data is ready
	UFUNCTION(BlueprintImplementableEvent)
	void OnMeleeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);
	
	UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

};
