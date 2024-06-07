#include "DamageSystem/DamageSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UDamageSystem::UDamageSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();

}

void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

ECanBeDamaged UDamageSystem::CanBeDamaged(bool ShouldDamageInvincible, bool CanBeBlocked)
{
	//implement later
	//if (!IsDead && ShouldDamageInvincible || !IsInvincible)
	if (!IsDead)
	{
		if (IsBlocking && CanBeBlocked)
		{
			return ECanBeDamaged::ECanBeDamaged_BlockedDamage;
		}
		else
		{
			return ECanBeDamaged::ECanBeDamaged_DoDamage;
		}
	}
	else
	{
		return ECanBeDamaged::ECanBeDamaged_NoDamage;
	}
}

float UDamageSystem::Heal(float HealAmount)
{
	if (!IsDead)
	{
		CurrentHealth += HealAmount;
		if (CurrentHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
		}
		return CurrentHealth;
	}
	return 0;
}

bool UDamageSystem::TakeDamage(FDamageInfo DamageInfo)
{

	DamageOutput = CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked);
	if (DamageOutput == ECanBeDamaged::ECanBeDamaged_BlockedDamage)
	{
		//CallOnBlocked(DamageInfo.CanBeParried)
		return true;
	}
	else if (DamageOutput == ECanBeDamaged::ECanBeDamaged_DoDamage)
	{
		CurrentHealth -= DamageInfo.DamageAmount;
		UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), CurrentHealth);

		if (CurrentHealth <= 0)
		{
			IsDead = true;
			OnDeathResponse.Execute();
			return true;
		}
		else
		{
			if (DamageInfo.ShouldForceInterrupt || IsInterruptible)
			{
				OnDamageResponse.Broadcast(DamageInfo);
				return true;
			}
			return true;
		}
	}
	else if (DamageOutput == ECanBeDamaged::ECanBeDamaged_NoDamage)
	{
		return false;
	}
	else
	{
		return false;
	}
}

void UDamageSystem::HandleHitReaction(FDamageInfo DamageInfo)
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}
		
	CharacterOwner->GetCharacterMovement()->StopMovementImmediately();

	UAnimMontage* HitReactionMontage = nullptr;

	switch (DamageInfo.FacingDirection)
	{
	case EFacingDirection::EFacingDirection_Left:
		HitReactionMontage = RightHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Right:
		HitReactionMontage = LeftHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Front:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Back:
		HitReactionMontage = BackHitReaction;
		break;

	case EFacingDirection::EFacingDirection_FrontLeft:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_FrontRight:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_BackLeft:
		HitReactionMontage = BackHitReaction;
		break;

	case EFacingDirection::EFacingDirection_BackRight:
		HitReactionMontage = BackHitReaction;
		break;

	default:
		break;
	}

	CharacterOwner->PlayAnimMontage(HitReactionMontage);
}
