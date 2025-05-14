#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

AFTAPlayerState::AFTAPlayerState(const FObjectInitializer& ObjectInitializer)
{
	FTAAbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("FTAAbilitySystemComponent"));
	FTAAbilitySystemComponent->SetIsReplicated(true);
	FTAAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

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
	
}

void AFTAPlayerState::BeginPlay()
{
	Super::BeginPlay();

}
