#include "TargetingComponents/TargetingSystemComponent.h"
#include "Characters/PlayableCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UTargetingSystemComponent::UTargetingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetingSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayableCharacter = Cast<APlayableCharacter>(GetOwner());

}

void UTargetingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsTargeting && HardTarget)
	{
		if (PlayableCharacter->GetDistanceTo(HardTarget) < 2000.f)
		{

			if (PlayableCharacter->GetCharacterMovement()->IsFalling() || PlayableCharacter->GetCharacterMovement()->IsFlying())
			{
				FVector ResultVectorAVector(0, 0, 80.f);
			}
			MidPoint = CalculateMidpoint(GetOwner()->GetActorLocation(), HardTarget->GetActorLocation());
			Radius = (CalculateDistance(GetOwner()->GetActorLocation(), HardTarget->GetActorLocation())) / 2;

			PlayableCharacter->LockOnSphere->SetWorldLocation(MidPoint);
			PlayableCharacter->SpringArmComp->TargetArmLength = Radius + 300;


			FVector ResultVector(0, 0, 0);
			FVector TargetLocation = HardTarget->GetActorLocation() - ResultVector;
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PlayableCharacter->GetActorLocation(), TargetLocation);
			FRotator InterpRot = FMath::RInterpTo(PlayableCharacter->GetControlRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);

			//PlayableCharacter->GetController()->SetControlRotation(InterpRot);
			PlayableCharacter->SetActorRotation(TargetRotation);
		}
		else
		{
			IsTargeting = false;
			HardTarget = NULL;
			PlayableCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
	
}

void UTargetingSystemComponent::HardLockOn()
{
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
				EDrawDebugTrace::None,
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

FVector UTargetingSystemComponent::CalculateMidpoint(FVector PlayerLocation, FVector TargetLocation)
{
	FVector Midpoint = (PlayerLocation + TargetLocation) / 2.0f;

	return Midpoint;
}

float UTargetingSystemComponent::CalculateDistance(FVector PlayerLocation, FVector TargetLocation)
{
	float CalculateX = FMath::Square(abs((PlayerLocation.X - TargetLocation.X)));
	float CalculateY = FMath::Square(abs((PlayerLocation.Y - TargetLocation.Y)));
	float CalculateZ = FMath::Square(abs((PlayerLocation.Z - TargetLocation.Z)));

	return FMath::Sqrt(CalculateX + CalculateY + CalculateZ);

}

