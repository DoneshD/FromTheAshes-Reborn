#include "Enemy/EnemyBaseCharacter.h"

#include "CombatComponents/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/EquipmentManagerComponent.h"


AEnemyBaseCharacter::AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(RootComponent);
	
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

	if(HealthWidget && HealthWidget->IsValidLowLevel())
	{
		HealthWidget->SetVisibility(false);	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthWidget is Null or Invalid"));
	}

	if(!EquipmentManagerComponent && !EquipmentManagerComponent->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentManagerComponent is Null or Invalid"));
		return;
	}
	
	EquipmentManagerComponent->SetEquippedWeapon(WeaponClass);

	if(!HealthComponent || !HealthWidget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is Null or Invalid"));
		return;
	}
	
	HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyBaseCharacter::HealthChanged);
	
}

void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
	SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
	
}

void AEnemyBaseCharacter::HealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator)
{
	OnHealthChanged(InHealthComponent, OldValue, NewValue, InInstigator);
}

void AEnemyBaseCharacter::CheckDeath(float NewValue)
{
	if(!IsDead && NewValue <= 0.0f)
	{
		IsDead = true;
		Death();
	}
}

void AEnemyBaseCharacter::Death()
{
	AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(GetController());

	if(!EnemyController && !EnemyController->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid enemy controller"));
		return;
	}

	EnemyController->BrainComponent->StopLogic("");
	
	if(!DeathMontage && !DeathMontage->IsValidLowLevel())
	{
		PlayAnimMontage(DeathMontage);
		
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
}



