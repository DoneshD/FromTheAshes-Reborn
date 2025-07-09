#include "FTACustomBase/FTACharacter.h"
#include "ComboManagerComponent.h"
#include "HealthComponent.h"
#include "MotionWarpingComponent.h"
#include "NiagaraComponent.h"
#include "WallRunningComponent.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/FTACharacterData.h"
#include "FTAAbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Weapon/EquipmentManagerComponent.h"

AFTACharacter::AFTACharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bAlwaysRelevant = true;

	FTAAbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthSet"));
	
	EquipmentManagerComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentManagerComponent");
	this->AddOwnedComponent(EquipmentManagerComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	this->AddOwnedComponent(HealthComponent);

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	this->AddOwnedComponent(MotionWarpingComponent);

	ComboManagerComponent = CreateDefaultSubobject<UComboManagerComponent>(TEXT("ComboManagerComponent"));
	this->AddOwnedComponent(ComboManagerComponent);

	AirCombatComponent = CreateDefaultSubobject<UAerialCombatComponent>(TEXT("AerialCombatComponent"));
	this->AddOwnedComponent(AirCombatComponent);
	
	WallRunningComponent = CreateDefaultSubobject<UWallRunningComponent>(TEXT("WallRunningComponent"));
	this->AddOwnedComponent(WallRunningComponent);

	CharacterAfterImageComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CharacterAfterImageComponent"));
	this->AddOwnedComponent(GetMesh());
	
}

void AFTACharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilitySystemComponent();
	AddCharacterBaseAbilities();
	
	if (!AirCombatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] APlayerCharacter::BeginPlay - AirCombatComponent is null"), *GetActorNameOrLabel());
		return;
	}
	HealthComponent->InitializeWithAbilitySystem(FTAAbilitySystemComponent);
}


UAbilitySystemComponent* AFTACharacter::GetAbilitySystemComponent() const
{
	return GetFTAAbilitySystemComponent();
}

TObjectPtr<UMotionWarpingComponent> AFTACharacter::GetMotionWarpingComponent() const
{
	if(!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::GetMotionWarpingComponent - MotionWarpingComponent is invalid"))
		return nullptr;
	}
	return MotionWarpingComponent;
}

void AFTACharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	bool AllTagsValid = UTagValidationFunctionLibrary::AllGameplayTagsRegisteredInContainer(TagContainer);
	
	if(!AllTagsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::GetOwnedGameplayTags - Tag(s) are invalid"))
		return;
	}
	
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		FTA_ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AFTACharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	bool AllTagsValid = UTagValidationFunctionLibrary::IsRegisteredGameplayTag(TagToCheck);
	
	if(!AllTagsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::HasMatchingGameplayTag - Tag(s) are invalid"))
		return false;
	}
	
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		return FTA_ASC->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

bool AFTACharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	bool AllTagsValid = UTagValidationFunctionLibrary::AllGameplayTagsRegisteredInContainer(TagContainer);
	
	if(!AllTagsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::HasAllMatchingGameplayTags - Tag(s) are invalid"))
		return false;
	}
	
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		return FTA_ASC->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AFTACharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	const bool AllTagsValid = UTagValidationFunctionLibrary::AllGameplayTagsRegisteredInContainer(TagContainer);
	
	if(!AllTagsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::HasAnyMatchingGameplayTags - Tag(s) are invalid"))
		return false;
	}
	
	if (const UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent())
	{
		return FTA_ASC->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
	
}

void AFTACharacter::InitAbilitySystemComponent()
{
	if(!FTAAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::InitAbilitySystemComponent - FTAAbilitySystemComponent is invalid"))
		return;
	}
	
	FTAAbilitySystemComponent->InitAbilityActorInfo(this, this);
	
}

void AFTACharacter::AddCharacterBaseAbilities() const
{
	if (!FTACharacterAbilitySetData)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] AddCharacterBaseAbilities: FTACharacterAbilitySetData is null"), *GetActorNameOrLabel());
		return;
	}

	if (FTACharacterAbilitySetData->CharacterAbilitySets.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] AddCharacterBaseAbilities: No AbilitySets defined"), *GetActorNameOrLabel());
	}

	for (const TObjectPtr<UFTAAbilitySet>& AbilitySet : FTACharacterAbilitySetData->CharacterAbilitySets)
	{
		if (!AbilitySet)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] AddCharacterBaseAbilities: Null AbilitySet in CharacterAbilitySets"), *GetActorNameOrLabel());
			continue;
		}

		AbilitySet->GiveToAbilitySystem(FTAAbilitySystemComponent, nullptr);
	}

	if (!FTAAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] AddCharacterBaseAbilities: FTAAbilitySystemComponent is null"), *GetActorNameOrLabel());
		return;
	}

	if (FTACharacterAbilitySetData->CharacterTagRelationshipMapping)
	{
		FTAAbilitySystemComponent->SetTagRelationshipMapping(FTACharacterAbilitySetData->CharacterTagRelationshipMapping);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] AddCharacterBaseAbilities: TagRelationshipMapping is null"), *GetActorNameOrLabel());
	}
}

void AFTACharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Launched.Vertical"));
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Slammed"));
	
	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")));
}

bool AFTACharacter::HasFlailTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));
}

bool AFTACharacter::HasLaunchedTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Launched.Vertical"));
}

bool AFTACharacter::HasSlammedTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Slammed"));
}

bool AFTACharacter::HasSlammingTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam"));
}



