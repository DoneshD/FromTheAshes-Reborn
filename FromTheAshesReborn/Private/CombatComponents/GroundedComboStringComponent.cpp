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
	
	for (const auto& Pair : ComboStringMap)
	{
		const FString& Key = Pair.Key;
		const TArray<TObjectPtr<UAnimMontage>>& AnimMontages = Pair.Value;

		if (CurrentComboString.Contains(Key.Mid(0, CurrentAttackIndex + 1)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Anim String: %s"), *Key);
			CurrentComboSeqAnim = Pair.Value;
			
			PerformCurrentAttack(CurrentAttackIndex);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("None!!!!"));

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

void UGroundedComboStringComponent::PerformCurrentAttack(int AttackIndex)
{
	UAnimMontage* CurrentAttackMontage = CurrentComboSeqAnim[AttackIndex];
	if (CurrentAttackMontage)
	{
		PC->SetState(EStates::EState_Attack);
		PC->SoftLockOn(250.0f);
		PC->PlayAnimMontage(CurrentAttackMontage);

		CurrentAttackIndex++;

		if (CurrentAttackIndex >= CurrentComboSeqAnim.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("RESET"));
			CurrentComboString = TEXT("");
			CurrentAttackIndex = 0;
		}
	}
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

