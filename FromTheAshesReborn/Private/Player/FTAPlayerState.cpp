#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

AFTAPlayerState::AFTAPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	// AttributeSet = CreateDefaultSubobject<UFTAAttributeSet>(TEXT("AttributeSet"));

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

	// check(AbilitySystemComponent);
	// UE_LOG(LogTemp, Warning, TEXT("AFTAPlayerState InitAbilityActorInfo OwnerActor: %s"), *this->GetName())
	// UE_LOG(LogTemp, Warning, TEXT("AFTAPlayerState InitAbilityActorInfo Avatar: %s"), *GetPawn->GetName())
	// GetFTAAbilitySystemComponent()->InitAbilityActorInfo(this, GetPawn());
}

void AFTAPlayerState::BeginPlay()
{
	Super::BeginPlay();

}
