#include "Enemy/EnemyBaseCharacter.h"
#include "DataAsset/FTACharacterData.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "Player/PlayerComboManagerComponent.h"


AEnemyBaseCharacter::AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerComboManagerComponent = CreateDefaultSubobject<UPlayerComboManagerComponent>(TEXT("PlayerComboManagerComponent"));
	this->AddOwnedComponent(PlayerComboManagerComponent);
	
}

void AEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FGameplayTagContainer& AEnemyBaseCharacter::GetCurrentComboContainer()
{
	return PlayerComboManagerComponent->GetCurrentComboContainer();
}

int AEnemyBaseCharacter::GetCurrentComboIndex()
{
	return PlayerComboManagerComponent->GetCurrentComboIndex();
}

void AEnemyBaseCharacter::SetCurrentComboIndex(int ComboIndex)
{
	PlayerComboManagerComponent->SetCurrentComboIndex(ComboIndex);
}
