#include "Enemy/AIControllerEnemyBase.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Enemy/EnemyBaseCharacter.h"


AAIControllerEnemyBase::AAIControllerEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	
	AISenseConfigDamage = CreateDefaultSubobject<UAISenseConfig_Damage>("SenseDamage");
	AISenseConfigDamage->SetMaxAge(5.0f);

	AIPerceptionComponent->ConfigureSense(*AISenseConfigDamage);
	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Damage::StaticClass());

	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIControllerEnemyBase::ActorsPerceptionUpdated);

	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAIComponent");

	
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
}

void AAIControllerEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAIControllerEnemyBase::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	
}
