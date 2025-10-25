#include "Enemy/EnemyBaseCharacter.h"

#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AEnemyBaseCharacter::AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	
}

void AEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGroupCombatSubsystem* GCS = GetWorld()->GetSubsystem<UGroupCombatSubsystem>();

	if(!GCS)
	{
		UE_LOG(LogTemp, Error, TEXT("GCS is Null"))
		return;
	}

	GCS->RegisterEnemyToGroupCombat(this);
	
}

void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
	SetActorRotation(LookAtRotation);
	
}


