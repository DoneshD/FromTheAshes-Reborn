
#include "GameModes/FromTheAshesRebornGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/EnemyMelee.h"
#include "Characters/EnemyBase.h"
#include "AIController.h"
#include "Characters/PlayableCharacter.h"
#include "EventManagers/EnemySpawner.h"
#include "UObject/ConstructorHelpers.h"

AFromTheAshesRebornGameMode::AFromTheAshesRebornGameMode()
{
	
}

void AFromTheAshesRebornGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	OnEnemySpawned.BindUObject(this, &AFromTheAshesRebornGameMode::IncrementEnemyCount);
	OnEnemyDeath.BindUObject(this, &AFromTheAshesRebornGameMode::CheckEnemyCount);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyBaseClass, EnemiesArray);

}

void AFromTheAshesRebornGameMode::PublishEnemySpawned()
{
	OnEnemySpawned.Execute();
}

void AFromTheAshesRebornGameMode::PublishEnemyDeath()
{
	OnEnemyDeath.Execute();
}

void AFromTheAshesRebornGameMode::SpawnMeleeEnemy()
{
	AActor* EnemySpawnerInstance = UGameplayStatics::GetActorOfClass(GetWorld(), EnemySpawnerClass);
	EnemySpawner = Cast<AEnemySpawner>(EnemySpawnerInstance);

	SpawnerLocation = EnemySpawner->GetRandomLocationAroundSpawner(1000.0f);


	EnemyMelee = GetWorld()->SpawnActorDeferred<AEnemyMelee>(EnemyMeleeClass, FTransform(
		SpawnRotation, SpawnerLocation), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	AAIControllerEnemyBase* AIController = GetWorld()->SpawnActor<AAIControllerEnemyBase>(SpawnParams);
	
	if (EnemyMelee)
	{
		if (AIController)
		{
			AIController->Possess(EnemyMelee);
			UGameplayStatics::FinishSpawningActor(EnemyMelee, FTransform(SpawnRotation, SpawnerLocation));

			EnemiesArray.Add(EnemyMelee);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn AI Controller"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Enemy Melee"));
	}
	
}

void AFromTheAshesRebornGameMode::ToggleEnemyAggro()
{
	if (IsAggroActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deactivating Aggro"));
		IsAggroActivated = false;

		for (TObjectPtr<AActor> Attacker : EnemiesArray)
		{
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
			AIEnemyInterface->Retreat();
		}
		return;
	}
	if (!IsAggroActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("Activating Aggro"));
		IsAggroActivated = true;

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		APlayableCharacter* PlayerCharacter = Cast<APlayableCharacter>(PlayerController->GetPawn());

		for (TObjectPtr<AActor> Attacker : EnemiesArray)
		{
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
			AIEnemyInterface->Attack(PlayerCharacter);
		}
		return;

	}
	
}

void AFromTheAshesRebornGameMode::TogglePlayerInvincibility()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayableCharacter* PlayerCharacter = Cast<APlayableCharacter>(PlayerController->GetPawn());

	if (IsPlayerInvincible)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deactivating Player Invincibility"));

		IsPlayerInvincible = false;
		
		PlayerCharacter->DamageSystemComponent->MaxHealth = 200;
		PlayerCharacter->DamageSystemComponent->CurrentHealth = 200;

		return;
	}
	if (!IsPlayerInvincible)
	{
		UE_LOG(LogTemp, Warning, TEXT("Activating Player Invincibility"));

		IsPlayerInvincible = true;

		PlayerCharacter->DamageSystemComponent->MaxHealth = 10000;
		PlayerCharacter->DamageSystemComponent->CurrentHealth = 10000;

		return;

	}
}

void AFromTheAshesRebornGameMode::ToggleAllInvincibility()
{

	if (IsAllInvicible)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deactivating All Invincibility"));

		IsAllInvicible = false;
		IsPlayerInvincible = true;

		TogglePlayerInvincibility();
		
		for (TObjectPtr<AActor> Attacker : EnemiesArray)
		{
			AEnemyBase* EnemyBase = Cast<AEnemyBase>(Attacker);
			EnemyBase->DamageSystemComponent->MaxHealth = 200;
			EnemyBase->DamageSystemComponent->CurrentHealth = 200;
		}
		return;
	}
	if (!IsAllInvicible)
	{
		UE_LOG(LogTemp, Warning, TEXT("Activating All Invincibility"));

		IsAllInvicible = true;
		IsPlayerInvincible = false;

		TogglePlayerInvincibility();

		for (TObjectPtr<AActor> Attacker : EnemiesArray)
		{
			AEnemyBase* EnemyBase = Cast<AEnemyBase>(Attacker);
			EnemyBase->DamageSystemComponent->MaxHealth = 10000;
			EnemyBase->DamageSystemComponent->CurrentHealth = 10000;
		}
		return;
	}
}

void AFromTheAshesRebornGameMode::IncrementEnemyCount()
{
	EnemiesCount += 1;
}

void AFromTheAshesRebornGameMode::CheckEnemyCount()
{
	EnemiesCount -= 1;

	if (EnemiesCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RIGHT All Enemies Eliminated"));
	}
}
