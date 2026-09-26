#include "FTAAbilitySystem/GameplayCues/OverlayCueNotify.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "FTAAbilitySystem/GameplayCues/MaterialCueObject.h"
#include "FTAAbilitySystem/GameplayCues/OverlayCueObject.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "VFX/CharacterOverlayComponent.h"

void AOverlayCueNotify::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters){
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	switch (EventType)
	{
	case EGameplayCueEvent::OnActive:
		{

			if (!Parameters.EffectContext.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("EffectContext is invalid"));
				break;
			}

			FFTAGameplayEffectContext* FTAContext = FFTAGameplayEffectContext::ExtractEffectContext(Parameters.EffectContext);

			if (!FTAContext)
			{
				UE_LOG(LogTemp, Error, TEXT("FTAContext is NULL"));
				break;
			}

			UFTACueObject* CueObject = FTAContext->CueObjects.FindRef(FGameplayTag::RequestGameplayTag("GameplayCue.Overlay"));

			OverlayCueObject = Cast<UOverlayCueObject>(CueObject);

			if (!OverlayCueObject)
			{
				return;
			}

			InitializeParameters();
			
			break;
		}

	case EGameplayCueEvent::WhileActive:
		{
			break;
		}

	case EGameplayCueEvent::Removed:
		{
			StopOverlay();
			break;
		}

	default:
		break;
	}
	
}

void AOverlayCueNotify::InitializeParameters()
{
	Character = Cast<ACharacter>(GetOwner());

	if(!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterOverlayComponent::BeginPlay() - Not a character"))
	}

	SkeletalMeshComponent = Character->GetMesh();

	IsActivated = false;

	if(!OverlayCueObject)
	{
		UE_LOG(LogTemp, Error, TEXT("No Object"))
		return;
	}

	OverlayCueObject->OverlayStruct.AlphaSpeed = OverlayCueObject->OverlayStruct.AlphaSpeed / 1000.0f;

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

	OverlayMaterialReference = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), OverlayCueObject->OverlayStruct.MaterialInterface);

	ActivateComponents();
}

void AOverlayCueNotify::ActivateComponents()
{
	UE_LOG(LogTemp, Log, TEXT("Activating OverlayCue"));
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
	&AOverlayCueNotify::StartOverlay, 
	.001f,                     
	true                     
	);
}

void AOverlayCueNotify::StartOverlay()
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
			Alpha = Alpha + OverlayCueObject->OverlayStruct.AlphaSpeed;
			OverlayMaterialReference->SetScalarParameterValue(FName("Fade"), Alpha);
		}
	}
}

void AOverlayCueNotify::StopOverlay()
{
	if(UseNiagaraGround)
	{
		GroundStartNiagaraComponent->Deactivate();
	}
	if(UseNiagaraOverlay)
	{
		SpawnedNiagaraComponent->Deactivate();
	}
	IsActivated = false;
	if(Alpha <= 0.0f)
	{
		Alpha = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	Alpha = Alpha - OverlayCueObject->OverlayStruct.AlphaSpeed;
	
	SkeletalMeshComponent->SetOverlayMaterial(nullptr);
	OverlayMaterialReference->SetScalarParameterValue(FName("Fade"), Alpha);
}
