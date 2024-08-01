#include "Weapons/FTAWeapon.h"
#include "FTAAbilitySystem/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/FTAGameplayAbility.h"
#include "FTACustomBase/FTACharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

AFTAWeapon::AFTAWeapon()
{
	// Set this actor to never tick
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh"));
	// //Not sure if right collision type
	// WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	// CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Manually enable when in pickup mode
	// CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// RootComponent = CollisionComp;

}

void AFTAWeapon::BeginPlay()
{
	Super::BeginPlay();

	//call equip here
	if (!OwningCharacter && bSpawnWithCollision)
	{
		// Spawned into the world without an owner, enable collision as we are in pickup mode
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

}

void AFTAWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


UAbilitySystemComponent* AFTAWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USkeletalMeshComponent* AFTAWeapon::GetWeaponMesh() const
{
	return WeaponMesh;
}

void AFTAWeapon::SetOwningCharacter(AFTACharacter* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		// Called when added to inventory
		AbilitySystemComponent = Cast<UFTAAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
		AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AFTAWeapon::Equip()
{
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}

	FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();

	if (WeaponMesh)
	{
		WeaponMesh->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh->SetRelativeLocation(WeaponMeshEquippedRelativeLocation);
		WeaponMesh->SetRelativeRotation(FRotator(0, 0, -90.0f));
	}
}

void AFTAWeapon::UnEquip()
{
	//Implement later
}

void AFTAWeapon::AddAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UFTAAbilitySystemComponent* ASC = Cast<UFTAAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("ASC is null"));
		return;
	}
	
	for (TSubclassOf<UFTAGameplayAbility>& Ability : Abilities)
	{
		AbilitySpecHandles.Add(ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, GetAbilityLevel(Ability.GetDefaultObject()->AbilityID), static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this)));
	}
}

void AFTAWeapon::RemoveAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UFTAAbilitySystemComponent* ASC = Cast<UFTAAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("ASC is null"));
		return;
	}
	
	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}
}

int32 AFTAWeapon::GetAbilityLevel(EGAbilityInputID AbilityID)
{
	return 1;
}

UAnimMontage* AFTAWeapon::GetEquipMontage() const
{
	return EquipMontage;
}

