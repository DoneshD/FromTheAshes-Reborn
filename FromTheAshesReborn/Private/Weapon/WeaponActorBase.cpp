#include "Weapon/WeaponActorBase.h"
#include "TracingComponent/TracingComponent.h"

AWeaponActorBase::AWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	TracingComponent = CreateDefaultSubobject<UTracingComponent>("TracingComponent");
	this->AddOwnedComponent(TracingComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	this->AddOwnedComponent(SkeletalMesh);

	NS_SwordTrail = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NS_SwordTrail->SetupAttachment(SkeletalMesh);
	
}

void AWeaponActorBase::BeginPlay()
{
	Super::BeginPlay();

	TracingComponent->SetupVariables(SkeletalMesh, nullptr);
	TracingComponent->MyActorsToIgnore.Add(this);
	TracingComponent->ShouldIgnoreSelf = true;
}

void AWeaponActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
