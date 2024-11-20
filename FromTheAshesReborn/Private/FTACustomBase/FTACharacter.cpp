#include "FTACustomBase/FTACharacter.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "DidItHitActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapons/WeaponManagerComponent.h"

AFTACharacter::AFTACharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bAlwaysRelevant = true;

	DidItHitActorComponent = CreateDefaultSubobject<UDidItHitActorComponent>("DiditHitActorComponent");
	WeaponManagerComponent = CreateDefaultSubobject<UWeaponManagerComponent>("WeaponManagerComponent");
}

void AFTACharacter::BeginPlay()
{
	Super::BeginPlay();
}


UAbilitySystemComponent* AFTACharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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

void AFTACharacter::AddDefaultAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->IsCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UFTAGameplayAbility>& StartupAbility : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->IsCharacterAbilitiesGiven = true;
}

void AFTACharacter::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}

}

void AFTACharacter::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->IsStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	AbilitySystemComponent->IsStartupEffectsApplied = true;
}

bool AFTACharacter::IsAlive() const
{
	return GetCurrentHealth() > 0.0f;
}

int32 AFTACharacter::GetAbilityLevel(EAbilityInputID AbilityID) const
{
	return 1;
}

void AFTACharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->IsCharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && DefaultAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->IsCharacterAbilitiesGiven = false;
}

void AFTACharacter::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AFTACharacter::FinishDying()
{
	Destroy();
}

int32 AFTACharacter::GetCharacterLevel() const
{
	return 1;
}

float AFTACharacter::GetCurrentHealth() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetCurrentHealth();
	}
	return 0.0f;
}

float AFTACharacter::GetMaxHealth() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMaxHealth();
	}
	
	return 0.0f;
}

float AFTACharacter::GetMoveSpeed() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMoveSpeed();
	}

	return 0.0f;
}

float AFTACharacter::GetMoveSpeedBaseValue() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSet)->GetBaseValue();
	}

	return 0.0f;
}

FName AFTACharacter::GetWeaponAttachPoint()
{
	return WeaponAttachPoint;
}


void AFTACharacter::SetCurrentHealth(float Health)
{
	if (IsValid(AttributeSet))
	{
		AttributeSet->SetCurrentHealth(Health);
	}
}

