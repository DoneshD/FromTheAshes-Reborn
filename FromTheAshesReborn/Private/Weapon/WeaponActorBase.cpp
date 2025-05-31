#include "Weapon/WeaponActorBase.h"
#include "DidItHitActorComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AWeaponActorBase::AWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DidItHitActorComponent = CreateDefaultSubobject<UDidItHitActorComponent>("DidItHitActorComponent");
	this->AddOwnedComponent(DidItHitActorComponent);

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
	DidItHitActorComponent->ToggleTraceCheck(true);
}

void AWeaponActorBase::EndWeaponTracing()
{
	
	DidItHitActorComponent->ToggleTraceCheck(false);
	DidItHitActorComponent->ClearHitArray();

}

