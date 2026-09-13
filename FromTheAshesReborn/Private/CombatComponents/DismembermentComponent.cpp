#include "CombatComponents/DismembermentComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UDismembermentComponent::UDismembermentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UDismembermentComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UDismembermentComponent::BeginPlay - OwnerCharacter invalid"));
		return;
	}
	
}

void UDismembermentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDismembermentComponent::CaptureDismembermentData(FName InBoneName)
{
	HitBoneName = InBoneName;
}

void UDismembermentComponent::RenameBoneName()
{
	if (HitBoneName == "pelvis" ||
	HitBoneName == "spine_02" ||
	HitBoneName == "spine_03" ||
	HitBoneName == "neck_01" ||
	HitBoneName == "head")
	{
		HitBoneName = "spine_01";
	}
	
}

void UDismembermentComponent::ApplyDismemberment(FVector InImpulse, FVector InHitLocation, FName InBoneName,
	UNiagaraSystem* InNiagaraSystem)
{
	OwnerCharacter->GetMesh()->BreakConstraint(InImpulse, OwnerCharacter->GetMesh()->GetSocketLocation(InBoneName), InBoneName);
	FTransform SocketTransform = OwnerCharacter->GetMesh()->GetSocketTransform(InBoneName);
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		InNiagaraSystem, OwnerCharacter->GetMesh(), InBoneName, SocketTransform.GetLocation(),
		SocketTransform.GetRotation().Rotator(), EAttachLocation::KeepWorldPosition, true);
}

