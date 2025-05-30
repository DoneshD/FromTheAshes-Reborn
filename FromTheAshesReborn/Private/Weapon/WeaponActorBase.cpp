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

void AWeaponActorBase::StartWeaponTrail()
{
	UE_LOG(LogTemp, Warning, TEXT("Start trail"))
	FVector StartLocation = SkeletalMesh->GetSocketLocation("VFX_Slash_Start");
	FVector EndLocation = SkeletalMesh->GetSocketLocation("VFX_Slash_End");

	FVector SpawnLocation = (StartLocation + EndLocation) / 2.0f;
	FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromZ(StartLocation - EndLocation);
	
	NC = UNiagaraFunctionLibrary::SpawnSystemAttached(SwordTrail, SkeletalMesh, FName(), SpawnLocation, SpawnRotation,EAttachLocation::KeepWorldPosition, false);

	float Length = (StartLocation - EndLocation).Length();
	NC->SetFloatParameter("TrailWidth", Length);
}

void AWeaponActorBase::EndWeaponTrail()
{
	UE_LOG(LogTemp, Warning, TEXT("end trail"))

	NC->Deactivate();
}

