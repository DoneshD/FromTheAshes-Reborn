#include "ComboSystemComponent.h"
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
	if (PC->IsLightAttackSaved)
	{
		PC->IsLightAttackSaved = false;
		if (PC->IsStateEqualToAny(MakeArray))
		{
			PC->SetState(EStates::EState_Nothing);
		}
		if (PC->IsSurgeAttackPaused)
		{
			PC->PerformComboSurge();
		}


		else if (PC->HeavyAttackIndex > 1)
		{
			PC->PerformComboFinisher(PC->ComboExtenderFinishers[3]);
		}
		else if (PC->ComboExtenderIndex > 0)
		{
			if (PC->BranchFinisher)
			{
				PC->PerformComboFinisher(PC->ComboExtenderFinishers[0]);
			}
			else if (PC->HeavyAttackIndex == 0)
			{
				PC->PerformComboExtender(PC->ComboExtenderIndex);
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
			PC->PerformComboFinisher(PC->ComboExtenderFinishers[2]);
		}
		else if (PC->LightAttackIndex == 2)
		{
			if (PC->ComboExtenderIndex == 0)
			{
				PC->PerformComboExtender(PC->ComboExtenderIndex);
			}
			else if (PC->BranchFinisher)
			{
				PC->PerformComboFinisher(PC->ComboExtenderFinishers[1]);
			}
		}
		else
		{
			PC->HeavyAttack();
		}
	}
}

void UComboSystemComponent::PerformComboExtender(int ExtenderIndex)
{
	UAnimMontage* CurrentMontage = PC->ComboExtender[ExtenderIndex];
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dodge };
	if (!PC->IsStateEqualToAny(MakeArray))
	{
		if (CurrentMontage)
		{
			PC->IsHeavyAttackPaused = false;
			PC->SetState(EStates::EState_Attack);
			PC->SoftLockOn(500.0f);
			PC->ComboExtenderIndex++;
			PC->PlayAnimMontage(CurrentMontage);

			if (PC->ComboExtenderIndex >= PC->ComboExtender.Num())
			{
				PC->BranchFinisher = true;
			}
			UE_LOG(LogTemp, Warning, TEXT("Extender"));

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
}

void UComboSystemComponent::PerformComboFinisher(UAnimMontage* FinisherMontage)
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dodge };
	if (!PC->IsStateEqualToAny(MakeArray))
	{
		if (FinisherMontage)
		{
			PC->IsHeavyAttackPaused = false;
			PC->ResetLightAttack();
			PC->ResetHeavyAttack();
			PC->SetState(EStates::EState_Attack);
			PC->SoftLockOn(500.0f);
			PC->PlayAnimMontage(FinisherMontage);
			UE_LOG(LogTemp, Warning, TEXT("Finisher"));
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

