
#include "TargetingComponent.h"
#include "Characters/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"


UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayableCharacter = Cast<APlayableCharacter>(GetOwner());
}


void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayableCharacter->IsTeleportActive)
	{
		if (UCameraComponent* FollowCamera = PlayableCharacter->CameraComp)
		{
			FVector CameraVector = FollowCamera->GetForwardVector();
			FVector EndLocation = (CameraVector * 2000.f) + PlayableCharacter->GetActorLocation();
			FHitResult OutHit;

			TArray<AActor*> ActorArray;
			ActorArray.Add(PlayableCharacter);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

			bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(),
				PlayableCharacter->GetActorLocation(),
				EndLocation,
				100.f,
				ObjectTypes,
				false,
				ActorArray,
				EDrawDebugTrace::None,
				OutHit,
				true);

			if (TargetHit)
			{
				AActor* HitActor = OutHit.GetActor();
				if (HitActor)
				{
					if (HitActor && HitActor != TeleportTarget)
					{
						TeleportTarget = HitActor;
						UE_LOG(LogTemp, Warning, TEXT("Teleport Target: %s"), *TeleportTarget->GetName());
					}
					else
					{

					}
				}
				else
				{
					TeleportTarget = NULL;
				}
			}
			else
			{
				TeleportTarget = NULL;
			}
		}
	}

	if (IsTargeting && HardTarget)
	{
		if (PlayableCharacter->GetDistanceTo(HardTarget) < 2000.f)
		{
			if (PlayableCharacter->GetCharacterMovement()->IsFalling() || PlayableCharacter->GetCharacterMovement()->IsFlying())
			{
				FVector ResultVectorAVector(0, 0, 80.f);
			}

			FVector ResultVector(0, 0, 0);
			FVector TargetLocation = HardTarget->GetActorLocation() - ResultVector;
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PlayableCharacter->GetActorLocation(), TargetLocation);
			FRotator InterpRot = FMath::RInterpTo(PlayableCharacter->GetControlRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.f);

			PlayableCharacter->GetController()->SetControlRotation(InterpRot);
		}
		else
		{
			IsTargeting = false;
			HardTarget = NULL;
			PlayableCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}

}

void UTargetingComponent::SoftLockOn(float Distance)
{
	UE_LOG(LogTemp, Warning, TEXT("In UTargetingComponent"));

	//APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(GetOwner());
	if (!IsTargeting && !HardTarget)
	{
		FVector StartLocation = (PlayableCharacter->GetActorLocation() + PlayableCharacter->GetCharacterMovement()->GetLastInputVector() * 100.f);
		FVector EndLocation = (PlayableCharacter->GetCharacterMovement()->GetLastInputVector() * Distance) + StartLocation;
		FHitResult OutHit;

		TArray<AActor*> ActorArray;
		ActorArray.Add(PlayableCharacter);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			StartLocation,
			EndLocation,
			50.f,
			ObjectTypes,
			false,
			ActorArray,
			EDrawDebugTrace::ForDuration,
			OutHit,
			true);

		if (TargetHit)
		{
			AActor* HitActor = OutHit.GetActor();
			if (HitActor)
			{
				SoftTarget = HitActor;
			}
		}
		else
		{
			SoftTarget = NULL;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Leaving UTargetingComponent"));

}

void UTargetingComponent::HardLockOn()
{
	UE_LOG(LogTemp, Warning, TEXT("In UTargetingComponent::HardLockOn"));
	if (!IsTargeting && !HardTarget)
	{

		if (UCameraComponent* FollowCamera = PlayableCharacter->CameraComp)
		{
			FVector CameraVector = FollowCamera->GetForwardVector();
			FVector EndLocation = (CameraVector * 2000.f) + PlayableCharacter->GetActorLocation();
			FHitResult OutHit;

			TArray<AActor*> ActorArray;
			ActorArray.Add(PlayableCharacter);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

			bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(),
				PlayableCharacter->GetActorLocation(),
				EndLocation,
				100.f,
				ObjectTypes,
				false,
				ActorArray,
				EDrawDebugTrace::ForDuration,
				OutHit,
				true);

			if (TargetHit)
			{
				AActor* HitActor = OutHit.GetActor();

				PlayableCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
				IsTargeting = true;
				HardTarget = HitActor;
			}
		}
	}
	else
	{
		IsTargeting = false;
		HardTarget = NULL;
		PlayableCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

