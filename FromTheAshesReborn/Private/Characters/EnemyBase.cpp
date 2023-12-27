// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "AIControllerEnemyBase.h"
#include "AIController.h"
#include "DamageSystem/DamageSystem.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DamageSystemComponent = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystemComponent"));
	this->AddOwnedComponent(DamageSystemComponent);

}
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AController* myEnemyController = this->GetController();
	AAIController* myAIEnemyController;
	if (myEnemyController)
	{
		myAIEnemyController = Cast<AAIController>(myEnemyController);
	}
	/*
	AController* EnemyController = this->GetController();
	AAIController* AIEnemyController;
	if (AIEnemyController)
	{
		AIEnemyController = Cast<AAIController>(EnemyController);
		if (AIEnemyController)
		{
			AICEnemyBase = Cast<AIControllerEnemyBase>(EnemyController);
		}
	}
	*/

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
	
	UE_LOG(LogTemp, Warning, TEXT("NativeTakeDamage() done"));
	return DamageSystemComponent->TakeDamage(DamageInfo);
}
