#include "Player/FTAPlayerState.h"

#include "DataAsset/FTACharacterData.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Player/PlayerCharacter.h"

AFTAPlayerState::AFTAPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UFTAAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* AFTAPlayerState::GetAbilitySystemComponent() const
{
	return GetFTAAbilitySystemComponent();
}

void AFTAPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AFTAPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void AFTAPlayerState::BeginPlay()
{
	Super::BeginPlay();
	AddAbilitiesToPlayerASC();

}

void AFTAPlayerState::AddAbilitiesToPlayerASC()
{
	APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn());

	for (const UFTAAbilitySet* AbilitySet : PC->FTACharacterData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}
