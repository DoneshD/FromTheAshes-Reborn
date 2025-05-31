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

void AWeaponActorBase::StartWeaponTrail(UNiagaraSystem* TestSystem)
{
	FVector StartLocation = SkeletalMesh->GetSocketLocation("VFX_Slash_Start");
	FVector EndLocation = SkeletalMesh->GetSocketLocation("VFX_Slash_End");
	
	SpawnTrailLocation = (StartLocation + EndLocation) / 2.0f;
	SpawnTrailRotation = UKismetMathLibrary::MakeRotFromZ(StartLocation - EndLocation);
	TrailLength = (StartLocation - EndLocation).Length();
	if(!TestSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("TestSystem"));
		return;
	}
	TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TestSystem, SkeletalMesh, FName(), SpawnTrailLocation, SpawnTrailRotation,EAttachLocation::KeepWorldPosition, false);
	TrailComponent->SetFloatParameter("TrailWidth", TrailLength);
}

void AWeaponActorBase::EndWeaponTrail()
{
	if(!TrailComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("TrailComponent is null"))
		return;
	}
	TrailComponent->Deactivate();
}

