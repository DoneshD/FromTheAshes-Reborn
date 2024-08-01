#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "FTACustomBase/FTAEnums.h"
#include "FTAWeapon.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "Weapon | Animation")
	UAnimMontage* GetEquipMontage() const;

protected:

	virtual void BeginPlay() override;
	
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY()
	UFTAAbilitySystemComponent* AbilitySystemComponent;
	
	// Collision capsule for when weapon is in pickup mode
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CollisionComp;

	//Probably change to static mesh 
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Base")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Base")
	FVector WeaponMeshEquippedRelativeLocation;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Weapon | Base")
	AFTACharacter* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "Ability System |Abilities")
	TArray<TSubclassOf<UFTAGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System |Abilities")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon | Animation")
	UAnimMontage* EquipMontage;
	
	// Cache tags
	FGameplayTag WeaponInstantAbilityTag;

};
