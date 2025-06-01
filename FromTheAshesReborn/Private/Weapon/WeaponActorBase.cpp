#include "Weapon/WeaponActorBase.h"
#include "DidItHitActorComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TracingComponent/TracingComponent.h"

AWeaponActorBase::AWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// DidItHitActorComponent = CreateDefaultSubobject<UDidItHitActorComponent>("DidItHitActorComponent");
	// this->AddOwnedComponent(DidItHitActorComponent);
	
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
	
	
}

void AWeaponActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponActorBase::StartWeaponTracing()
{
	// DidItHitActorComponent->ToggleTraceCheck(true);
	TracingComponent->ToggleTraceCheck(true);
}

void AWeaponActorBase::EndWeaponTracing()
{
	
	// DidItHitActorComponent->ToggleTraceCheck(false);
	// DidItHitActorComponent->ClearHitArray();

	TracingComponent->ToggleTraceCheck(false);
	TracingComponent->ClearHitArray();

}

