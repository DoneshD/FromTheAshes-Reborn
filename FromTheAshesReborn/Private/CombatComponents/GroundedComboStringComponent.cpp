#include "CombatComponents/GroundedComboStringComponent.h"
#include "Characters/PlayableCharacter.h"
#include "TimerManager.h"
UGroundedComboStringComponent::UGroundedComboStringComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGroundedComboStringComponent::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayableCharacter>(GetOwner());

	CurrentComboString = TEXT("");

	ComboStringMap.Add(Combo1, PC->LightAttackCombo);
	ComboStringMap.Add(Combo2, PC->HeavyAttackCombo);
	ComboStringMap.Add(Combo3, PC->ComboSeq3);
	ComboStringMap.Add(Combo4, PC->ComboSeq4);
	ComboStringMap.Add(Combo5, PC->ComboSeq5);
	ComboStringMap.Add(Combo6, PC->ComboSeq6);
	ComboStringMap.Add(Combo7, PC->ComboSeq7);
	ComboStringMap.Add(Combo8, PC->ComboSeq8);
	ComboStringMap.Add(Combo9, PC->ComboSeq9);
	ComboStringMap.Add(Combo10, PC->ComboSeq10);
}

void UGroundedComboStringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UGroundedComboStringComponent::SelectComboString()
{
	UE_LOG(LogTemp, Log, TEXT("CurrentComboString: %s"), *CurrentComboString);
	
	for (const auto& Pair : ComboStringMap)
	{
		const FString& Key = Pair.Key;
		const TArray<TObjectPtr<UAnimMontage>>& AnimMontages = Pair.Value;

		if (CurrentComboString.Contains(Key.Mid(0, CurrentAttackIndex + 1)))
		{
			CurrentComboSeqAnim = Pair.Value;
			PerformCurrentAttack(CurrentAttackIndex);
			return;
		}
		else
		{

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

void UGroundedComboStringComponent::StartAttackPauseTimer()
{
	GetWorld()->GetTimerManager().SetTimer(FAttackPauseHandle, this, &UGroundedComboStringComponent::AppendAttackPause, .9, true);
}

void UGroundedComboStringComponent::ClearAttackPauseTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FAttackPauseHandle);
	CurrentComboString.RemoveFromEnd(CurrentComboString);

}

void UGroundedComboStringComponent::AppendAttackPause()
{
	CurrentComboString.Append(TEXT("P"));
}

void UGroundedComboStringComponent::AppendLightAttack()
{
	if (PC->CanAttack())
	{
		CurrentComboString.Append(TEXT("L"));
		SelectComboString();
	}
	else
	{
		return;
	}
}

void UGroundedComboStringComponent::AppendHeavyAttack()
{
	
	if (PC->CanAttack())
	{
		CurrentComboString.Append(TEXT("H"));
		SelectComboString();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	}
}

void UGroundedComboStringComponent::PerformCurrentAttack(int AttackIndex)
{
	UAnimMontage* CurrentAttackMontage = CurrentComboSeqAnim[AttackIndex];
	if (CurrentAttackMontage)
	{
		PC->SetState(EStates::EState_Attack);
		PC->SoftLockOn(250.0f);
		PC->PlayAnimMontage(CurrentAttackMontage);
		StartAttackPauseTimer();

		CurrentAttackIndex++;

		if (CurrentAttackIndex >= CurrentComboSeqAnim.Num())
		{
			ClearAttackPauseTimer();
			CurrentComboString = TEXT("");
			CurrentAttackIndex = 0;
		}
	}
}
