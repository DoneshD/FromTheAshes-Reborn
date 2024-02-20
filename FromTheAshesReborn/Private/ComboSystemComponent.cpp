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
	PlayableCharacter = Cast<APlayableCharacter>(GetOwner());
}

void UComboSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UComboSystemComponent::SaveLightAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (PlayableCharacter->IsLightAttackSaved)
	{
		PlayableCharacter->IsLightAttackSaved = false;
		if (PlayableCharacter->IsStateEqualToAny(MakeArray))
		{
			PlayableCharacter->SetState(EStates::EState_Nothing);
		}
		if (PlayableCharacter->IsSurgeAttackPaused)
		{
			PlayableCharacter->PerformComboSurge();
		}


		else if (PlayableCharacter->HeavyAttackIndex > 1)
		{
			PlayableCharacter->PerformComboFinisher(PlayableCharacter->ComboExtenderFinishers[3]);
		}
		else if (PlayableCharacter->ComboExtenderIndex > 0)
		{
			if (PlayableCharacter->BranchFinisher)
			{
				PlayableCharacter->PerformComboFinisher(PlayableCharacter->ComboExtenderFinishers[0]);
			}
			else if (PlayableCharacter->HeavyAttackIndex == 0)
			{
				PlayableCharacter->PerformComboExtender(PlayableCharacter->ComboExtenderIndex);
			}
		}
		else
		{
			PlayableCharacter->LightAttack();
		}
	}
}

void UComboSystemComponent::SaveHeavyAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (PlayableCharacter->IsHeavyAttackSaved)
	{
		PlayableCharacter->IsHeavyAttackSaved = false;
		if (PlayableCharacter->IsStateEqualToAny(MakeArray))
		{
			PlayableCharacter->SetState(EStates::EState_Nothing);
		}
		if (PlayableCharacter->IsHeavyAttackPaused)
		{
			PlayableCharacter->NewHeavyCombo();
		}
		else if (PlayableCharacter->LightAttackIndex == 3)
		{
			PlayableCharacter->PerformComboFinisher(PlayableCharacter->ComboExtenderFinishers[2]);
		}
		else if (PlayableCharacter->LightAttackIndex == 2)
		{
			if (PlayableCharacter->ComboExtenderIndex == 0)
			{
				PlayableCharacter->PerformComboExtender(PlayableCharacter->ComboExtenderIndex);
			}
			else if (PlayableCharacter->BranchFinisher)
			{
				PlayableCharacter->PerformComboFinisher(PlayableCharacter->ComboExtenderFinishers[1]);
			}
		}
		else
		{
			PlayableCharacter->HeavyAttack();
		}
	}
}

