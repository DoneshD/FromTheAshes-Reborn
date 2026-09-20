#include "VFX/CharacterOverlayComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMaterialLibrary.h"

UCharacterOverlayComponent::UCharacterOverlayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterOverlayComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());

	if(!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterOverlayComponent::BeginPlay() - Not a character"))
	}

	SkeletalMeshComponent = Character->GetMesh();

	IsActivated = false;

	TimerSpeed = TimerSpeed / 1000.0f;

	if(UseNiagaraGround)
	{
		GroundStartNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraGround,
			SkeletalMeshComponent,
			FName("None"), FVector(0.0f, 0.0f, 0.0f),
			FRotator(0.0f, 0.0f, 0.0f),
			EAttachLocation::KeepRelativeOffset,
			false,
			false,
			ENCPoolMethod::ManualRelease,
			true);
		
	}

	if(UseNiagaraOverlay)
	{
		SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraOverlay,
			SkeletalMeshComponent,
			FName("None"), FVector(0.0f, 0.0f, 0.0f),
			FRotator(0.0f, 0.0f, 0.0f),
			EAttachLocation::KeepRelativeOffset,
			false,
			false,
			ENCPoolMethod::ManualRelease,
			true);


		UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(SpawnedNiagaraComponent, FString("Skeletal Mesh"), SkeletalMeshComponent);

	}

	OverlayMaterialReference = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(),OverlayMaterial);
	
	
}

void UCharacterOverlayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCharacterOverlayComponent::ToggleOverlay()
{
	if(IsActivated)
	{
		SkeletalMeshComponent->SetOverlayMaterial(OverlayMaterialReference);
		if(Alpha >= 1.0f)
		{
			Alpha = 1.0f;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			TimerHandle.Invalidate();
		}
		else
		{
			Alpha = Alpha + TimerSpeed;
			OverlayMaterialReference->SetScalarParameterValue(FName("Fade"), Alpha);
		}
		
	}
}

void UCharacterOverlayComponent::StartEffect()
{

	if(UseNiagaraGround)
	{
		GroundStartNiagaraComponent->Activate();
	}
	if(UseNiagaraOverlay)
	{
		SpawnedNiagaraComponent->Activate();
	}

	IsActivated = true;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,            
		this,                      
		&UCharacterOverlayComponent::ToggleOverlay, 
		.001f,                     
		true                     
	);
}
