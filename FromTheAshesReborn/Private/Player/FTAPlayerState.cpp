#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

AFTAPlayerState::AFTAPlayerState(const FObjectInitializer& ObjectInitializer)
{
	FTAAbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("FTAAbilitySystemComponent"));
	FTAAbilitySystemComponent->SetIsReplicated(true);
	FTAAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	UE_LOG(LogTemp, Display, TEXT("AFTAPlayerState::AFTAPlayerState()"));
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
