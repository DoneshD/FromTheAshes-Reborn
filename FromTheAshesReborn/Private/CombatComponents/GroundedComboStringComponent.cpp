#include "CombatComponents/GroundedComboStringComponent.h"
#include "Characters/PlayableCharacter.h"

UGroundedComboStringComponent::UGroundedComboStringComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGroundedComboStringComponent::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayableCharacter>(GetOwner());

	CurrentComboString = TEXT("");

	ComboSequences.Add(Combo1);
	ComboSequences.Add(Combo2);
	ComboSequences.Add(Combo3);


	ComboStringMap.Add(Combo1, PC->LightAttackCombo);
	ComboStringMap.Add(Combo2, PC->HeavyAttackCombo);
	ComboStringMap.Add(Combo3, PC->ComboSeq3);
	ComboStringMap.Add(Combo4, PC->ComboSeq4);

}

void UGroundedComboStringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UGroundedComboStringComponent::SelectComboString()
{
	UE_LOG(LogTemp, Log, TEXT("CurrentComboString: %s"), *CurrentComboString);

	if (CurrentAttackIndex < 1 && CurrentComboString == TEXT("L"))
	{
		CurrentComboSequence = PC->LightAttackCombo;
		PerformLightAttack(CurrentAttackIndex);
		return;
	}
	if (CurrentAttackIndex < 1 && CurrentComboString == TEXT("H"))
	{
		CurrentComboSequence = PC->HeavyAttackCombo;
		PerformLightAttack(CurrentAttackIndex);
		return;
	}
	
	for (const auto& Pair : ComboStringMap)
	{
		const FString& Key = Pair.Key;
		const TArray<TObjectPtr<UAnimMontage>>& AnimMontages = Pair.Value;

		if (Key.Contains(CurrentComboString))
		{
			CurrentComboSequence = Pair.Value;
			PerformLightAttack(CurrentAttackIndex);
			return;
		}
			
	}
}

void UGroundedComboStringComponent::SaveLightAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };

	if (IsLightAttackSaved)
	{
		IsLightAttackSaved = false;

		if (PC->IsStateEqualToAny(MakeArray))
		{
			PC->SetState(EStates::EState_Nothing);
		}
		CurrentComboString.Append(TEXT("L"));
		SelectComboString();
	}
}

void UGroundedComboStringComponent::SaveHeavyAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };

	if (IsHeavyAttackSaved)
	{
		IsHeavyAttackSaved = false;

		if (PC->IsStateEqualToAny(MakeArray))
		{
			PC->SetState(EStates::EState_Nothing);
		}
		CurrentComboString.Append(TEXT("H"));
		SelectComboString();
	}
}


void UGroundedComboStringComponent::LightAttack()
{
	if (PC->CanAttack())
	{
		PC->ResetHeavyAttack();
		CurrentComboString.Append(TEXT("L"));
		SelectComboString();
	}
	else
	{
		return;
	}
}


void UGroundedComboStringComponent::PerformCurrentAttack(float WarpingDistance, TObjectPtr<UAnimMontage> AttackMontage)
{
		PC->SetState(EStates::EState_Attack);
		PC->SoftLockOn(WarpingDistance);
		PC->PlayAnimMontage(AttackMontage);
	
}

void UGroundedComboStringComponent::PerformLightAttack(int AttackIndex)
{
	UAnimMontage* CurrentAttackMontage = CurrentComboSequence[AttackIndex];
	if (CurrentAttackMontage)
	{
		PerformCurrentAttack(250.0f, CurrentAttackMontage);
		CurrentAttackIndex++;
		if (CurrentAttackIndex >= CurrentComboSequence.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("RESET"));
			CurrentComboString = TEXT("");
			CurrentAttackIndex = 0;
		}
	}
}


void UGroundedComboStringComponent::HeavyAttack()
{
	

	if (PC->CanAttack())
	{
		PC->ResetLightAttack();

		CurrentComboString.Append(TEXT("H"));
		SelectComboString();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	}

}

void UGroundedComboStringComponent::PrintCurrentComboString()
{
	UE_LOG(LogTemp, Log, TEXT("CurrentComboString: %s"), *CurrentComboString);
}