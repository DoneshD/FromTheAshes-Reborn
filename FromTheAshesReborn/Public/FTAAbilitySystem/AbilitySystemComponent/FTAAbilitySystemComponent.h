#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UFTAAbilitySystemComponent*, SourceASC, float,
                                               UnmitigatedDamage, float, MitigatedDamage);

class USkeletalMeshComponent;
class UFTAAT_PlayMontageAndWaitForEvent;


UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

//protected member variabes
public:
	bool IsCharacterAbilitiesGiven = false;
	bool IsStartupEffectsApplied = false;

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
	
	FReceivedDamageDelegate ReceivedDamage;
	
//public functions
public:
	
	UFTAAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	// Exposes GetTagCount to Blueprint
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities | Gameplaytags", Meta = (DisplayName = "GetTagCount", ScriptName = "GetTagCount"))
	int32 K2_GetTagCount(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass, UObject* OptionalSourceObject=nullptr);

	// Exposes AddLooseGameplayTag to Blueprint. This tag is *not* replicated.
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "AddLooseGameplayTag"))
	void K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	// Exposes AddLooseGameplayTags to Blueprint. These tags are *not* replicated.
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "AddLooseGameplayTags"))
	void K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	// Exposes RemoveLooseGameplayTag to Blueprint. This tag is *not* replicated.
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "RemoveLooseGameplayTag"))
	void K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	// Exposes RemoveLooseGameplayTags to Blueprint. These tags are *not* replicated.
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "RemoveLooseGameplayTags"))
	void K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCue(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCue(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCue(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);
	
	
	// Called from FTADamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	// Plays a montage and handles replication and prediction based on passed in ability/activation info
	virtual float PlayMontageForMesh(UGameplayAbility* AnimatingAbility, class USkeletalMeshComponent* InMesh, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, bool bReplicateMontage = true);
	
public:
	

	// Version of function in AbilitySystemGlobals that returns correct type
	static UFTAAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

	// Input bound to an ability is pressed
	virtual void AbilityLocalInputPressed(int32 InputID) override;
	
};