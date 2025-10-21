#include "Enemy/EnemyGruntCharacter.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"

AEnemyGruntCharacter::AEnemyGruntCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AEnemyGruntCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyGruntCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

