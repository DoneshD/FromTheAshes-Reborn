#include "Enemy/AIControllerEnemyBase.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Kismet/GameplayStatics.h"


AAIControllerEnemyBase::AAIControllerEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAIComponent");
	
}

void AAIControllerEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AAIControllerEnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyBaseCharacter* EnemyCharacter = Cast<AEnemyBaseCharacter>(InPawn);
	
	if (!EnemyCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] AAIControllerEnemyBase::OnPossess - EnemyCharacter is null"), *GetActorNameOrLabel());
		return;
	}

	OwnerPawn = InPawn;
}

void AAIControllerEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if(!PlayerTarget || !PlayerTarget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Player Target not set"));
		return;
	}
	
}