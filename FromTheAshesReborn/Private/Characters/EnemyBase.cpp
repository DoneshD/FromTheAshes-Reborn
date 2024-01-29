// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionParry->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnBoxBeginOverlap);
	CollisionParry->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::OnBoxEndOverlap);

	ParryMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ParryMesh"));
	ParryMesh->SetupAttachment(GetMesh());
}
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AController* EnemyController = this->GetController();
	if (EnemyController)
	{
		AAIController* AIEnemyController = Cast<AAIController>(EnemyController);
		if (AIEnemyController)
		{
			AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(AIEnemyController);
			if (AIControllerEnemyBase)
			{
				AICEnemyBase = AIControllerEnemyBase;
			}
		}
	}

	//Bind Death Event
	//Bind Hit Response Event
	
}


void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::NativeGetCurrentHealth()
{
	return DamageSystemComponent->CurrentHealth;
}

float AEnemyBase::NativeGetMaxHealth()
{
	return DamageSystemComponent->MaxHealth;
}

bool AEnemyBase::NativeIsDead()
{
	return DamageSystemComponent->IsDead;
}

float AEnemyBase::NativeHeal(float HealAmount)
{
	return DamageSystemComponent->Heal(HealAmount);
}

bool AEnemyBase::NativeTakeDamage(FDamageInfo DamageInfo)
{
	
	UE_LOG(LogTemp, Warning, TEXT("AEnemyBase::NativeTakeDamage"));
	return DamageSystemComponent->TakeDamage(DamageInfo);
}

float AEnemyBase::NativeSetMovementSpeed(EMovementSpeed SpeedState)
{
	switch (SpeedState)
	{
	case EMovementSpeed::EMovementSpeed_Idle:
		return GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	case EMovementSpeed::EMovementSpeed_Walking:
		return GetCharacterMovement()->MaxWalkSpeed = 100.0f;

	case EMovementSpeed::EMovementSpeed_Jogging :
		return GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	case EMovementSpeed::EMovementSpeed_Sprinting:
		return GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	default:
		return 0.0f;
		break;
	}
}

void AEnemyBase::NativeGetIdealRange(float& OutAttackRadius, float& OutDefendRadius)
{
	
}

void AEnemyBase::JumpToDestination(FVector Destination)
{
	FVector LaunchVelocity;
	FVector EndDestination = Destination + FVector(0.0f, 0.0f, 250.0f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVelocity, GetActorLocation(), Destination);
	LaunchCharacter(LaunchVelocity, true, true);
}

void AEnemyBase::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(OtherActor);
	if (DamagableInterface)
	{
		DamagableInterface->WithinParryRange = true;
	}
	
}

void AEnemyBase::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(OtherActor);
	if (DamagableInterface)
	{
		DamagableInterface->WithinParryRange = false;
	}
}
