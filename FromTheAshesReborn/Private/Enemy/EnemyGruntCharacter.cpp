#include "Enemy/EnemyGruntCharacter.h"

#include "Enemy/AIControllerEnemyGrunt.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"

AEnemyGruntCharacter::AEnemyGruntCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AEnemyGruntCharacter::BeginPlay()
{
	Super::BeginPlay();

	// if(GetController())
	// {
	// 	AAIControllerEnemyGrunt* GruntController = Cast<AAIControllerEnemyGrunt>(GetController());
	//
	// 	if(GruntController && GruntController->IsValidLowLevel())
	// 	{
	// 		GruntController->Possess(this);
	// 		GruntController->StateTreeComponent->StartLogic();
	// 	}
	// }
	
}

void AEnemyGruntCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

