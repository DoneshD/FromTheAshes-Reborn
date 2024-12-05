#include "FTACustomBase/FTACharacter.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/EquipmentManagerComponent.h"

AFTACharacter::AFTACharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bAlwaysRelevant = true;

	AbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	EquipmentManagerComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentManagerComponent");
	this->AddOwnedComponent(EquipmentManagerComponent);

}

void AFTACharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AFTACharacter Name: %d"), AbilitySystemComponent->GetUniqueID());
}


UAbilitySystemComponent* AFTACharacter::GetAbilitySystemComponent() const
{
	return GetFTAAbilitySystemComponent();
}

void AFTACharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		FTA_ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AFTACharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		return FTA_ASC->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

bool AFTACharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		return FTA_ASC->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AFTACharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		return FTA_ASC->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}

