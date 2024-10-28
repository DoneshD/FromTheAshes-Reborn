#include "ParkourSystem/ParkourSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ParkourSystem/ParkourFunctionLibrary.h"
#include "ParkourSystem/ArrowActor.h"
#include "ParkourSystem/WidgetActor.h"

UParkourSystemComponent::UParkourSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}


void UParkourSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	
	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCharacter is NULL"));
		return;
	}
	
	CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>());
	
	if(!CharacterMovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMovementComponent is NULL"));
		return;
	}
}


void UParkourSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UParkourSystemComponent::ParkourInputPressedVault()
{
	ParkourAction(false);
}

bool UParkourSystemComponent::SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom,
	UCameraComponent* Camera, UMotionWarpingComponent* MotionWarping)
{
	
	OwnerCharacter = Character;
	
	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCharacter is NULL"));
		return false;
	}
	
	CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>());
	
	if(!CharacterMovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMovementComponent is NULL"));
		return false;
	}

	SkeletalMeshComponent = Cast<USkeletalMeshComponent>(OwnerCharacter->FindComponentByClass<USkeletalMeshComponent>());

	if(!SkeletalMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("SkeletalMesh is NULL"));
		return false;
	}

	AnimInstance = SkeletalMeshComponent->GetAnimInstance();

	if(!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AnimInstance is NULL"));
		return false;
	}

	CapsuleComponent = OwnerCharacter->GetCapsuleComponent();

	if(!CapsuleComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CapsuleComponent is NULL"));
		return false;
	}

	CameraBoomComponent = CameraBoom;

	if(!CameraBoomComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraBoomComponent is NULL"));
		return false;
	}

	CameraComponent = Camera;

	if(!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraComponent is NULL"));
		return false;
	}

	MotionWarpingComponent = MotionWarping;
	
	if(!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MotionWarpingComponent is NULL"));
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;

	FTransform SpawnTransform(OwnerCharacter->GetActorTransform());
	
	WidgetActor = GetWorld()->SpawnActor<AWidgetActor>(WidgetActorClass, SpawnTransform, SpawnParams);

	if(!WidgetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetActor is NULL"));
		return false;
	}

	FAttachmentTransformRules WidgetTransformRules(EAttachmentRule::SnapToTarget, true);

	WidgetActor->AttachToComponent(CameraComponent, WidgetTransformRules);
	WidgetActor->SetActorRelativeLocation(FVector(14.0f, 8.0, -3.0));
	
	FAttachmentTransformRules TransformRules(EAttachmentRule::KeepRelative, true);
	ArrowActor = GetWorld()->SpawnActor<AArrowActor>(ArrowActorClass, SpawnTransform, SpawnParams);

	if(!ArrowActor)
	{
		UE_LOG(LogTemp, Error, TEXT("ArrowActor is NULL"));
		return false;
	}
	ArrowActor->AttachToComponent(SkeletalMeshComponent, TransformRules);
	ArrowActor->SetActorRelativeLocation(FVector(ArrowLocationX, 0, ArrowLocationZ - CharacterHeightDiff));

	if(!CameraBoomComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraBoomComponent is NULL"));
		return false;
	}

	FirstTargetArmLength = CameraBoomComponent->TargetArmLength;
	FirstArmRelativeLocation = CameraBoomComponent->GetRelativeLocation();
	
	return false;
}

bool UParkourSystemComponent::SelectClimb(bool InCanManualClimb, bool InCanAutoClimb, bool InAutoClimb)
{
	return InAutoClimb ? InCanAutoClimb : InCanManualClimb;
}

void UParkourSystemComponent::ParkourAction(bool InAutoClimb)
{
	if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction")))
	{
		if(SelectClimb(CanManualClimb, CanAutoClimb, AutoClimb))
		{
			ParkourCheckWallShape();
		}
	}
}

void UParkourSystemComponent::ParkourCheckWallShape()
{
	bool bIsFalling = CharacterMovementComponent->IsFalling();

	int32 Index = bIsFalling ? 8 : 15;

	for (int32 i = 0; i <= Index; i++)
	{
		for(int32 k = 0; k <= 11; k++)
		{
			FVector ZLocation = FVector(0.0f, 0.0f, i * 16) + FVector(0.0f, 0.0f, -60.0f) + OwnerCharacter->GetActorLocation();
			FVector ForwardInverseVector = OwnerCharacter->GetActorForwardVector() * -20.0f;
			
			FVector StartLocation = ZLocation + ForwardInverseVector;
			FVector EndLocation = ZLocation + (OwnerCharacter->GetActorForwardVector() * (10.0f + (k * 10.0f)));
			
			FCollisionShape SphereShape = FCollisionShape::MakeSphere(10.0f);
			FHitResult OutHitResult;
			
			bool bHit = GetWorld()->SweepSingleByChannel(OutHitResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, SphereShape);

			DrawDebugSphere(GetWorld(), StartLocation, 10.0f, 4, FColor::Green, true, 5.0f);
			DrawDebugSphere(GetWorld(), EndLocation, 10.0f, 4, FColor::Red, true, 5.0f);

			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 5.0f, 0, 2.0f);

			if (bHit)
			{
				DrawDebugSphere(GetWorld(), OutHitResult.Location, 10.0f * 0.5f, 12, FColor::Yellow, true, 5.0f);
			}

			if(OutHitResult.bBlockingHit && !OutHitResult.bStartPenetrating)
			{
				WallHitTraces.Empty();
				
				float ParkourStateFloat = UParkourFunctionLibrary::SelectParkourStateFloat(4.0f, 0.0f, 0.0f, 2.0f, ParkourStateTag);
				for(int j = 0; j <= UKismetMathLibrary::FTrunc(ParkourStateFloat); j++)
				{
					float IndexStateFloat = UParkourFunctionLibrary::SelectParkourStateFloat(-40.0f, 0.0f, 0.0f, -20.0f, ParkourStateTag);
					
					FRotator ReverseRotationZ = UParkourFunctionLibrary::NormalReverseRotationZ(OutHitResult.ImpactNormal);
					FVector LastVector = ReverseRotationZ.Vector().RightVector * FVector((j * 20) + IndexStateFloat, (j * 20) + IndexStateFloat, (j * 20) + IndexStateFloat); 
					
					
					float FirstZValue = ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")) ? 0 : -60.0;
					float SecondZValue = ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")) ? OutHitResult.ImpactPoint.Z : OwnerCharacter->GetActorLocation().Z;

					FVector FirstVector = FVector(0.0f, 0.0f, FirstZValue) + FVector(OutHitResult.ImpactPoint.X, OutHitResult.ImpactPoint.Y, SecondZValue) + LastVector;
					FVector StartLocationLine = FirstVector + ReverseRotationZ.Vector().ForwardVector * -40.0f;
					FVector EndLocationLine = FirstVector + ReverseRotationZ.Vector().ForwardVector * 30.0f;

					FHitResult OutHitLineResult;
					bool bHitLine = GetWorld()->LineTraceSingleByChannel(OutHitLineResult, StartLocationLine, EndLocationLine, ECC_Visibility);

					DrawDebugSphere(GetWorld(), FirstVector, 10.0f, 4, FColor::Yellow, false, 5.0f);
    
					// Draw debug point where the line trace hits, if bHitLine is true
					if (bHitLine)
					{
						DrawDebugPoint(GetWorld(), OutHitLineResult.ImpactPoint, 10.0f, FColor::Red, false, 5.0f);
					}

					HopHitResult.Empty();
					float SelectParkourStateFloat = UParkourFunctionLibrary::SelectParkourStateFloat(30.0f, 0.0f, 0.0f, 7.0f, ParkourStateTag);
					for (int m = 0; m <= UKismetMathLibrary::FTrunc(SelectParkourStateFloat); m++)
					{
						FHitResult OutHitLineResultSecond;
						UE_LOG(LogTemp, Warning, TEXT("HELLLLOO!!"))
						// Calculate the start and end locations for the line trace
						FVector StartLocationLineSecond = FVector(0.0f, 0.0f, m * 8) + OutHitLineResult.TraceStart;
						FVector EndLocationLineSecond = FVector(0.0f, 0.0f, m * 8) + OutHitLineResult.TraceEnd;

						// Perform the line trace
						bool bHitLineSecond = GetWorld()->LineTraceSingleByChannel(OutHitLineResultSecond, StartLocationLineSecond, EndLocationLineSecond, ECC_Visibility);
						HopHitResult.Add(OutHitLineResultSecond);

						// Draw the debug line for visualization
						DrawDebugLine(GetWorld(), StartLocationLineSecond, EndLocationLineSecond, FColor::Green, true, 2.0f, 0, 1.0f);

						// If a hit is detected, draw a debug point at the hit location
						if (bHitLineSecond)
						{
							DrawDebugPoint(GetWorld(), OutHitLineResultSecond.ImpactPoint, 20.0f, FColor::Red, true, 2.0f);
						}
					}


				}
				
				break;
			}
		}
		break;
	}
}

