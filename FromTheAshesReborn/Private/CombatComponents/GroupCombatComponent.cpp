#include "CombatComponents/GroupCombatComponent.h"

UGroupCombatComponent::UGroupCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGroupCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	StartRoleTimer();
}

void UGroupCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UGroupCombatComponent::ReserveAttackToken(int32 Amount)
{
	if(AttackTokensCount >= Amount)
	{
		AttackTokensCount -= Amount;
		return true;
	}
	return false;
}

void UGroupCombatComponent::ReturnAttackToken(int32 Amount)
{
	AttackTokensCount += Amount;
}

void UGroupCombatComponent::DoSomething()
{
	UE_LOG(LogTemp, Warning, TEXT("UGroupCombatComponent::DoSomething"));
	
	UGroupCombatSubsystem* GCS = GetWorld()->GetSubsystem<UGroupCombatSubsystem>();
	if(GCS)
	{
		AEnemyBaseCharacter* EnemyOwner = Cast<AEnemyBaseCharacter>(GetOwner());
		if(EnemyOwner)
		{
			if(EngagementRole == EEnemyEngagementRole::Cover)
			{
				GCS->AssignEngagementRole(EnemyOwner, EEnemyEngagementRole::Aggressor);
			}
			else if(EngagementRole == EEnemyEngagementRole::Aggressor)
			{
				GCS->AssignEngagementRole(EnemyOwner, EEnemyEngagementRole::Cover);
			}
			
		}
	}
}

void UGroupCombatComponent::StartRoleTimer()
{
	
	RoleTimerStartTime = GetWorld()->GetTimeSeconds();
	
	GetWorld()->GetTimerManager().SetTimer(
		CurrentRoleTimer,
		this,
		&UGroupCombatComponent::DoSomething,
		5.0f,
		false
	);
}
