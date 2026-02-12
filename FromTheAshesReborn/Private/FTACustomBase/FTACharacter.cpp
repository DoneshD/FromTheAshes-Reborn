#include "FTACustomBase/FTACharacter.h"

#include "ContextualAnimSceneActorComponent.h"
#include "FTAMotionWarpingComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "CombatComponents/HealthComponent.h"
#include "MotionWarpingComponent.h"
#include "NiagaraComponent.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "CombatComponents/AfterImageComponent.h"
#include "CombatComponents/CentralStateComponent.h"
#include "CombatComponents/CombatTracingComponent.h"
#include "CombatComponents/DownedCombatComponent.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/FTACharacterData.h"
#include "FTAAbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/EquipmentManagerComponent.h"
#include "Weapon/WeaponActorBase.h"

AFTACharacter::AFTACharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bAlwaysRelevant = true;

	FTAAbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthSet"));

	CentralStateComponent = CreateDefaultSubobject<UCentralStateComponent>(TEXT("CentralStateComponent"));
	this->AddOwnedComponent(CentralStateComponent);
	InitializedActorComponents.AddUnique(CentralStateComponent);
	
	EquipmentManagerComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentManagerComponent");
	this->AddOwnedComponent(EquipmentManagerComponent);
	InitializedActorComponents.AddUnique(EquipmentManagerComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	this->AddOwnedComponent(HealthComponent);
	InitializedActorComponents.AddUnique(HealthComponent);

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	this->AddOwnedComponent(MotionWarpingComponent);
	InitializedActorComponents.AddUnique(MotionWarpingComponent);

	FTAMotionWarpingComponent = CreateDefaultSubobject<UFTAMotionWarpingComponent>(TEXT("FTAMotionWarpingComponent"));
	this->AddOwnedComponent(FTAMotionWarpingComponent);
	InitializedActorComponents.AddUnique(FTAMotionWarpingComponent);

	CombatTracingComponent = CreateDefaultSubobject<UCombatTracingComponent>(TEXT("CombatTracingComponent"));
	this->AddOwnedComponent(CombatTracingComponent);
	InitializedActorComponents.AddUnique(CombatTracingComponent);
	
	ComboManagerComponent = CreateDefaultSubobject<UComboManagerComponent>(TEXT("ComboManagerComponent"));
	this->AddOwnedComponent(ComboManagerComponent);
	InitializedActorComponents.AddUnique(ComboManagerComponent);

	AirCombatComponent = CreateDefaultSubobject<UAerialCombatComponent>(TEXT("AerialCombatComponent"));
	this->AddOwnedComponent(AirCombatComponent);
	InitializedActorComponents.AddUnique(AirCombatComponent);

	GroupCombatComponent = CreateDefaultSubobject<UGroupCombatComponent>(TEXT("GroupCombatComponent"));
	this->AddOwnedComponent(GroupCombatComponent);
	InitializedActorComponents.AddUnique(GroupCombatComponent);

	DownedCombatComponent = CreateDefaultSubobject<UDownedCombatComponent>(TEXT("DownedCombatComponent"));
	this->AddOwnedComponent(DownedCombatComponent);
	InitializedActorComponents.AddUnique(DownedCombatComponent);
	
	CharacterAfterImageComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CharacterAfterImageComponent"));
	this->AddOwnedComponent(GetMesh());
	InitializedActorComponents.AddUnique(CharacterAfterImageComponent);

	AfterImageComponent = CreateDefaultSubobject<UAfterImageComponent>(TEXT("AfterImageComponent"));
	this->AddOwnedComponent(GetMesh());
	InitializedActorComponents.AddUnique(AfterImageComponent);

	// ContextualAnimSceneActorComponent = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(TEXT("ContextualAnimSceneActorComponent"));
	// this->AddOwnedComponent(GetMesh());
	// InitializedActorComponents.AddUnique(ContextualAnimSceneActorComponent);
	
}

void AFTACharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilitySystemComponent();
	AddCharacterBaseAbilities();
	
	HealthComponent->InitializeWithAbilitySystem(FTAAbilitySystemComponent);
	
	CentralStateComponent->SetCurrentState(CentralStateComponent->NeutralStateTag);
	CentralStateComponent->SetCurrentOrientation(CentralStateComponent->GroundedOrientationTag, MOVE_Walking);

}

void AFTACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void AFTACharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if(!CheckForInvalidComponents())
	{
		ensureMsgf(false, TEXT("[%s] AFTACharacter::PostInitializeComponents() failed validation. Check the log for details."),
			*GetActorNameOrLabel());
	}
	
}

UAbilitySystemComponent* AFTACharacter::GetAbilitySystemComponent() const
{
	return GetFTAAbilitySystemComponent();
}

TObjectPtr<UMotionWarpingComponent> AFTACharacter::GetMotionWarpingComponent() const
{
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

bool AFTACharacter::CheckForInvalidComponents()
{
	bool bAllComponentsValid = true;

	for(int32 i = 0; i < InitializedActorComponents.Num(); i++)
	{
		TObjectPtr<UActorComponent> Component = InitializedActorComponents[i];

		if(!Component)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] AFTACharacter::CheckForInvalidComponents() - Invalid Component at index %d: Pointer is NULL"),
				*GetActorNameOrLabel(), i);
			
			bAllComponentsValid = false;
			continue;
		}

		if(!Component->IsValidLowLevel())
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] AFTACharacter::CheckForInvalidComponents() - Invalid Component at index %d: %s is not a valid UObject"),
				*GetActorNameOrLabel(), i, *Component->GetName());
			
			bAllComponentsValid = false;
		}
	}
	return bAllComponentsValid;
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
	
	if(!CentralStateComponent || !CentralStateComponent->IsValidLowLevel())
	{
		return;
	}
	
	CentralStateComponent->SetCurrentOrientation(CentralStateComponent->GroundedOrientationTag, MOVE_Walking);

	RemoveAerialEffects();
}

void AFTACharacter::Falling()
{
	Super::Falling();

	// if(!CentralStateComponent || !CentralStateComponent->IsValidLowLevel())
	// {
	// 	return;
	// }
	//
	// if(CentralStateComponent && CentralStateComponent->IsValidLowLevel())
	// {
	// 	CentralStateComponent->SetCurrentOrientation(CentralStateComponent->AirborneOrientationTag, MOVE_Falling);
	// }

}

bool AFTACharacter::HasFlailTag() const
{
	bool bHasTag = FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));
	return bHasTag;
}

bool AFTACharacter::HasLaunchedTag() const
{
	bool bHasTag = FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Launched.Vertical"));
	return bHasTag;
}


bool AFTACharacter::HasSlammedTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Slammed"));
}

bool AFTACharacter::HasSlammingTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam.Light"));
}

bool AFTACharacter::HasSlammingHeavyTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam.Heavy"));
	
}

bool AFTACharacter::HasDownedTag() const
{
	return FTAAbilitySystemComponent && FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Character.State.Downed"));
}

void AFTACharacter::SetUseLeftHandIK(bool InBool)
{
	bUseLeftHandIK = InBool;
}

FTransform AFTACharacter::GetLHIKTransform()
{
	//Implemented with only single weapons right now
	if(EquipmentManagerComponent->GetEquippedWeaponActors()[0])
	{
		FTransform SocketTransform = EquipmentManagerComponent->GetEquippedWeaponActors()[0]->SkeletalMesh->GetSocketTransform("LHIK");
		FVector OutLocation;
		FRotator OutRotation;

		GetMesh()->TransformToBoneSpace("hand_r", SocketTransform.GetLocation(), SocketTransform.Rotator(), OutLocation, OutRotation);
		return UKismetMathLibrary::MakeTransform(OutLocation, OutRotation);
	}
	return FTransform();
}

void AFTACharacter::RemoveAerialEffects()
{
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Launched.Vertical"));
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Slammed"));
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent"));
	
	
	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")));
}



