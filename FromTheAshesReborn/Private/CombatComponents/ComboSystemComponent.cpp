#include "CombatComponents/ComboSystemComponent.h"
#include "Enums/EStates.h"
#include "Characters/PlayableCharacter.h"

UComboSystemComponent::UComboSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UComboSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	PC = Cast<APlayableCharacter>(GetOwner());
}

void UComboSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UComboSystemComponent::SaveLightAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	TArray<EStates> DashArray = { EStates::EState_Dash };
	if (PC->IsLightAttackSaved)
	{

		PC->IsLightAttackSaved = false;
		if (PC->IsStateEqualToAny(DashArray))
		{
			UE_LOG(LogTemp, Warning, TEXT("Dash Attack"));
			PC->MeleeAttackLogicComponent->ExtendAttackRange = true;
		}

		if (PC->IsStateEqualToAny(MakeArray))
		{
			UE_LOG(LogTemp, Warning, TEXT("PC->SetState(EStates::EState_Nothing);"));

			PC->SetState(EStates::EState_Nothing);
		}

		if (PC->IsSurgeAttackPaused)
		{
			PerformComboSurge();
		}
		else if (PC->HeavyAttackIndex > 1)
		{
			PerformComboFinisher(PC->ComboExtenderFinishers[3]);
		}
		else if (PC->ComboExtenderIndex > 0)
		{
			if (PC->BranchFinisher)
			{
				PerformComboFinisher(PC->ComboExtenderFinishers[0]);
			}
			else if (PC->HeavyAttackIndex == 0)
			{
				PerformComboExtender(PC->ComboExtenderIndex);
			}
		}
		else
		{
			PC->LightAttack();
		}
	}
}

void UComboSystemComponent::SaveHeavyAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (PC->IsHeavyAttackSaved)
	{
		PC->IsHeavyAttackSaved = false;
		if (PC->IsStateEqualToAny(MakeArray))
		{
			PC->SetState(EStates::EState_Nothing);
		}
		if (PC->IsHeavyAttackPaused)
		{
			PC->NewHeavyCombo();
		}
		else if (PC->LightAttackIndex == 3)
		{
			PerformComboFinisher(PC->ComboExtenderFinishers[2]);
		}
		else if (PC->LightAttackIndex == 2)
		{
			if (PC->ComboExtenderIndex == 0)
			{
				PerformComboExtender(PC->ComboExtenderIndex);
			}
			else if (PC->BranchFinisher)
			{
				PerformComboFinisher(PC->ComboExtenderFinishers[1]);
			}
		}
		else
		{
			PC->HeavyAttack();
		}
	}
}

void UComboSystemComponent::PerformCurrentAttack(float WarpingDistance, TObjectPtr<UAnimMontage> AttackMontage)
{
	PC->SetState(EStates::EState_Attack);
	PC->SoftLockOn(WarpingDistance);
	PC->PlayAnimMontage(AttackMontage);
}

void UComboSystemComponent::PerformLightAttack(int AttackIndex)
{
	UAnimMontage* CurrentAttackMontage = PC->LightAttackCombo[AttackIndex];
	if (CurrentAttackMontage)
	{
		PerformCurrentAttack(250.0f, CurrentAttackMontage);
		PC->ResetSurgeCombo();
		PC->LightAttackIndex++;
		if (PC->LightAttackIndex >= PC->LightAttackCombo.Num())
		{
			PC->LightAttackIndex = 0;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
}

void UComboSystemComponent::PerformHeavyAttack(int HeavyAttackIndex)
{
	UAnimMontage* CurrentAttackMontage = PC->HeavyAttackCombo[PC->HeavyAttackIndex];
	if (CurrentAttackMontage)
	{
		PerformCurrentAttack(500.0f, CurrentAttackMontage);
		PC->StartHeavyAttackPausedTimer();
		if (HeavyAttackIndex == 0)
		{
			PC->StartSurgeAttackPausedTimer();
		}
		else
		{
			PC->ClearSurgeAttackPausedTimer();
			PC->IsSurgeAttackPaused = false;
		}
		PC->HeavyAttackIndex++;
		if (PC->HeavyAttackIndex >= PC->HeavyAttackCombo.Num())
		{
			PC->HeavyAttackIndex = 0;
			PC->ClearHeavyAttackPausedTimer();
			PC->IsHeavyAttackPaused = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
}

void UComboSystemComponent::PerformHeavyPauseCombo(TArray<TObjectPtr<UAnimMontage>> PausedHeavyAttackCombo)
{
	UAnimMontage* CurrentAttackMontage = PausedHeavyAttackCombo[PC->NewHeavyAttackIndex];
	if (CurrentAttackMontage)
	{
		PerformCurrentAttack(250.0f, CurrentAttackMontage);
		PC->NewHeavyAttackIndex++;
		if (PC->NewHeavyAttackIndex >= PausedHeavyAttackCombo.Num())
		{
			PC->NewHeavyAttackIndex = 0;
			PC->IsHeavyAttackPaused = false;
		}
	}
}

void UComboSystemComponent::PerformComboExtender(int ExtenderIndex)
{
	UAnimMontage* CurrentAttackMontage = PC->ComboExtender[ExtenderIndex];
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };
	if (!PC->IsStateEqualToAny(MakeArray))
	{
		if (CurrentAttackMontage)
		{

			PC->IsHeavyAttackPaused = false;
			PerformCurrentAttack(500.0f, CurrentAttackMontage);
			PC->ComboExtenderIndex++;

			if (PC->ComboExtenderIndex >= PC->ComboExtender.Num())
			{
				PC->BranchFinisher = true;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
}

void UComboSystemComponent::PerformComboFinisher(TObjectPtr<UAnimMontage> FinisherMontage)
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };
	if (!PC->IsStateEqualToAny(MakeArray))
	{
		if (FinisherMontage)
		{
			PC->IsHeavyAttackPaused = false;
			PC->ResetLightAttack();
			PC->ResetHeavyAttack();
			PerformCurrentAttack(500.0f, FinisherMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
		}
	}
	else
	{
		return;
	}
}

void UComboSystemComponent::PerformComboSurge()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };

	if (!PC->IsStateEqualToAny(MakeArray))
	{
		PC->IsHeavyAttackPaused = false;
		PC->ResetLightAttack();
		PC->ResetHeavyAttack();
		PC->SetState(EStates::EState_Attack);
		PC->SoftLockOn(500.0f);
		PC->PlayAnimMontage((PC->ComboSurgeCount % 2 == 0) ? PC->ComboSurge_L : PC->ComboSurge_R, PC->ComboSurgeSpeed);
		PC->ComboSurgeCount += 1;
		PC->ComboSurgeSpeed = (PC->ComboSurgeCount > 5) ? 1.6 : (PC->ComboSurgeCount > 2) ? 1.4 : 1.2;
	}
	else
	{
		return;
	}
}

