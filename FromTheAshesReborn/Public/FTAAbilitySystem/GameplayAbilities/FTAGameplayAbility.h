#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FTACustomBase/FTAEnums.h"
#include "FTAAbilitySystem/AbilityTypes/FTAAbilityTypes.h"
#include "FTAGameplayAbility.generated.h"

class AFTACharacter;
class AFTAPlayerController;
class UPlayerComboManagerComponent;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FAbilityComboDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityComboClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InputSavedTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ComboWindowTag;
};


UENUM(BlueprintType)
enum class EFTAAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

UENUM(BlueprintType)
enum class EFTAAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking

	//MAX	UMETA(Hidden)
};

UCLASS()
class FROMTHEASHESREBORN_API UFTAGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UFTAAbilitySystemComponent;

	


//functions
public:
	UFTAGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	AFTAPlayerController* GetFTAPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	AFTACharacter* GetFTACharacterFromActorInfo() const;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Combo")
	FAbilityComboDataStruct AbilityComboDataStruct;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	EAbilityInputID AbilityID = EAbilityInputID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateAbilityOnGranted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateOnInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bSourceObjectMustEqualCurrentWeaponToActivate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bCannotActivateWhileInteracting;

	// Map of gameplay tags to gameplay effect containers
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayEffects")
	TMap<FGameplayTag, FFTAGameplayEffectContainer> EffectContainerMap;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult> HitResults);

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FFTAGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FFTAGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FFTAGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FFTAGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability", meta = (DisplayName = "Get Source Object"))
	UObject* K2_GetSourceObject(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	bool FTACheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	
	virtual bool FTACheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void FTAApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;
	
	virtual void FTAApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const {};

	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool IsInputPressed() const;
	


protected:

};
