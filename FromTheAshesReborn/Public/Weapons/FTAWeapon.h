#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "FTACustomBase/FTAEnums.h"
#include "FTAWeapon.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);


class AFTACharacter;
class UAnimMontage;
class UFTAAbilitySystemComponent;
class UFTAGameplayAbility;
class USkeletalMeshComponent;

UCLASS(Blueprintable, BlueprintType)
class FROMTHEASHESREBORN_API AFTAWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFTAWeapon();

	// Whether or not to spawn this weapon with collision enabled (pickup mode).
	// Set to false when spawning directly into a player's inventory or true when spawning into the world in pickup mode.
	UPROPERTY(BlueprintReadWrite)
	bool bSpawnWithCollision;

	// This tag is primarily used by the first person Animation Blueprint to determine which animations to play
	// (Rifle vs Rocket Launcher)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASShooter|GSWeapon")
	FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASShooter|GSWeapon")
	FGameplayTagContainer RestrictedPickupTags;
	
	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASShooter|GSWeapon")
	virtual USkeletalMeshComponent* GetWeaponMesh() const;

	void SetOwningCharacter(AFTACharacter* InOwningCharacter);

	// Called when the player equips this weapon
	virtual void Equip();

	// Called when the player unequips this weapon
	virtual void UnEquip();

	virtual void AddAbilities();

	virtual void RemoveAbilities();

	virtual int32 GetAbilityLevel(EGAbilityInputID AbilityID);

	// Resets things like fire mode to default
	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSWeapon")
	virtual void ResetWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void OnDropped(FVector NewLocation);
	virtual void OnDropped_Implementation(FVector NewLocation);
	virtual bool OnDropped_Validate(FVector NewLocation);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|Animation")
	UAnimMontage* GetEquipMontage() const;
	

protected:
	UPROPERTY()
	UFTAAbilitySystemComponent* AbilitySystemComponent;
	
	// Collision capsule for when weapon is in pickup mode
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "GASShooter|GSWeapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GASShooter|GSWeapon")
	AFTACharacter* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "GASShooter|GSWeapon")
	TArray<TSubclassOf<UFTAGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "GASShooter|GSWeapon")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|Animation")
	UAnimMontage* EquipMontage;
	
	// Cache tags
	FGameplayTag WeaponInstantAbilityTag;
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

};