#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

class UAerialCombatComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UHealthComponent;
class ULauncherComponent;
class AWeaponActorBase;
class UFTAAbilitySystemComponent;
class UFTACharacterData;
class UEquipmentManagerComponent;
class UMotionWarpingComponent;
class UComboManagerComponent;
class UWallRunningComponent;
class UDidItHitActorComponent;


UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;
	
public:
	
	UPROPERTY(EditAnywhere, Category = "FTACharacter | Ability Set")
	TObjectPtr<UFTACharacterData> FTACharacterAbilitySetData;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UEquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	// UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "FTACharacter | Core Components")
	// TObjectPtr<UAerialCombatComponent> AerialCombatComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "FTACharacter | Core Components")
	TObjectPtr<UAerialCombatComponent> AirCombatComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UWallRunningComponent> WallRunningComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UNiagaraComponent> CharacterAfterImageComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FTACharacter | Niagara")
	UNiagaraSystem* FX_AfterImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FTACharacter | Effects")
	TSubclassOf<AActor> AfterImageActor;
	
	UPROPERTY(BlueprintReadWrite)
	bool DisableRootMotion = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsDead = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsAlreadyDead = false;
	
public:
	
	AFTACharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Character|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const  { return FTAAbilitySystemComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TObjectPtr<UMotionWarpingComponent> GetMotionWarpingComponent() const;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void InitAbilitySystemComponent();

	void AddCharacterBaseAbilities() const;

	virtual void Landed(const FHitResult& Hit) override;

	//TODO: Allplaceholder below
	UFUNCTION(BlueprintPure)
	bool HasFlailTag() const;

	UFUNCTION(BlueprintPure)
	bool HasLaunchedTag() const;

	UFUNCTION(BlueprintPure)
	bool HasSlammedTag() const;

	UFUNCTION(BlueprintPure)
	bool HasSlammingTag() const;

	UFUNCTION(BlueprintImplementableEvent)
	void StartHitStop(float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	void EndHitStop();
	
};