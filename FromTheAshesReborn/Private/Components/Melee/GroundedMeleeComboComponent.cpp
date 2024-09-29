#include "Components/Melee/GroundedMeleeComboComponent.h"
#include  "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "GameplayTagContainer.h"
#include "Player/PlayerCharacter.h"


UGroundedMeleeComboComponent::UGroundedMeleeComboComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGroundedMeleeComboComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UGroundedMeleeComboComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UGroundedMeleeComboComponent::PrintCurrentComboContainer()
{
	for (const FGameplayTag& Tag : CurrentComboTagContainer)
	{
		if(*Tag.ToString())
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentComboTagContainer Tag: %s"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentComboTagContainer Tag is NULL"));
		}
	}
}

FGameplayTagContainer& UGroundedMeleeComboComponent::GetCurrentComboContainer()
{
	return CurrentComboTagContainer;
}

int UGroundedMeleeComboComponent::GetCurrentComboIndex()
{
	return CurrentComboIndex;
}

void UGroundedMeleeComboComponent::SetCurrentComboIndex(int Index)
{
	CurrentComboIndex = Index;
}

