﻿#include "ParkourSystem/ParkourSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
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
			FindParkourLocationAndShape();
		}
	}
}


void UParkourSystemComponent::FindParkourLocationAndShape()
{
	FVector CapsuleStartLocation;
	FVector CapsuleEndLocation;
	float CapsuleRadius;
	float CapsuleHalfHeight;
	const TArray<AActor*> ActorsToIgnore;
	FHitResult FirstCapsuleTraceOutHitResult;
	
	GetFirstCapsuleTraceSettings(CapsuleStartLocation, CapsuleEndLocation, CapsuleRadius, CapsuleHalfHeight);
	bool bFirstCapsuleTraceOutHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CapsuleStartLocation, CapsuleEndLocation, CapsuleRadius, CapsuleHalfHeight, TraceTypeQuery1, false,
		ActorsToIgnore, EDrawDebugTrace::ForDuration, FirstCapsuleTraceOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	if(FirstCapsuleTraceOutHitResult.bBlockingHit)
	{
		WallHitTraces.Empty();
		for(int32 i = 0; i <= HorizontalWallDetectTraceHalfQuantity * 2; i++)
		{
			
			HopHitTraces.Empty();
			for(int32 j = 0; j <= VerticalWallDetectTraceQuantity/ (CharacterMovementComponent->IsFalling() ? 2 : 1); j++)
			{
				//example for adding two floats to make a vector
				FVector Vector1 = FVector(i * HorizontalWallDetectTraceRange + HorizontalWallDetectTraceRange * HorizontalWallDetectTraceHalfQuantity * -1.0f);
				FRotator NormalizeCapusleRotation = UParkourFunctionLibrary::NormalReverseRotationZ(FirstCapsuleTraceOutHitResult.ImpactNormal);
				FVector Vector2 = Vector1 * UKismetMathLibrary::GetRightVector(NormalizeCapusleRotation);
				FVector Vector3 = FVector(FirstCapsuleTraceOutHitResult.ImpactPoint.X, FirstCapsuleTraceOutHitResult.ImpactPoint.Y, GetVerticalWallDetectStartHeight());
				FVector TripleVector = FVector(0.0f, 0.0f, j * VerticalWallDetectTraceRange) + Vector2 + Vector3;

				//HopHit Traces
				FVector StartHopHitTracesLocation = TripleVector + (UKismetMathLibrary::GetForwardVector(NormalizeCapusleRotation) * -40.0f);
				FVector EndHopHitTracesLocation = TripleVector + (UKismetMathLibrary::GetForwardVector(NormalizeCapusleRotation) * 30.0f);
				FHitResult HopHitOutHitResult;
				FCollisionQueryParams test;
				bool HopHitOutHit =  GetWorld()->LineTraceSingleByChannel(HopHitOutHitResult, StartHopHitTracesLocation, EndHopHitTracesLocation, ECC_Visibility);
				HopHitTraces.Add(HopHitOutHitResult);
			}
			int32 k = 0;
			for (FHitResult OutHopHitResult : HopHitTraces)
			{
				if(k != 0)
				{
					float HitDistance1 = FVector::Dist(OutHopHitResult.TraceStart, OutHopHitResult.TraceEnd);
					float SelectFloat1 = OutHopHitResult.bBlockingHit ? OutHopHitResult.Distance : HitDistance1;

					float HitDistance2 = FVector::Dist(HopHitTraces[k - 1].TraceStart, HopHitTraces[k - 1].TraceEnd);
					float SelectFloat2 = HopHitTraces[k - 1].bBlockingHit ? HopHitTraces[k - 1].Distance : HitDistance2;

					if(SelectFloat1 - SelectFloat2 > 5.0f)
					{
						WallHitTraces.Add(HopHitTraces[k - 1]);
					}
				}
				k++;
			}
		}
		int32 l = 0;
		for (FHitResult OutWallHitResult : WallHitTraces)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hello"));
			if(l == 0)
			{
				WallHitResult = OutWallHitResult;
			}
			else
			{
				if(FVector::Dist(OutWallHitResult.ImpactPoint, OwnerCharacter->GetActorLocation()) <= FVector::Dist(WallHitResult.ImpactPoint, OwnerCharacter->GetActorLocation()))
				{
					WallHitResult = OutWallHitResult;
				}
				else
				{
					WallHitResult = WallHitResult;
				}
			}
			l++;
		}
	}
}

void UParkourSystemComponent::GetFirstCapsuleTraceSettings(FVector& OutStart, FVector& OutEnd, float& OutRadius,
	float& OutHalfHeight)
{
	static FGameplayTag NotBusyTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.NotBusy"));
	static FGameplayTag ClimbTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.Climb"));

	if(ParkourStateTag.MatchesTag(NotBusyTag))
	{
		
	}
	else if(ParkourStateTag.MatchesTag(ClimbTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Parkour state"));
	}

	FVector TempStart = OwnerCharacter->GetActorLocation() + FVector(
		0.0f, 0.0f, CharacterMovementComponent->IsFalling() ? 15.0f : 75.0f);

	FVector Velocity = CharacterMovementComponent->Velocity.GetSafeNormal2D(0.0001);
	float OutRangeA = CharacterMovementComponent->IsFalling() ? 25.0f : 50.0f;
	float OutRangeB = CharacterMovementComponent->IsFalling() ? 100.0f : 200.0f;

	float ClampedRange = UKismetMathLibrary::MapRangeClamped(Velocity.Length(), 0.0f, 500.0f, OutRangeA, OutRangeB);

	FVector TempEnd = TempStart + (FVector(Velocity.X, Velocity.Y, 0.0f) * ClampedRange);

	OutStart = TempStart;
	OutEnd = TempEnd;
	OutRadius = 40.0f;
	OutHalfHeight = CharacterMovementComponent->IsFalling() ? 70.0f : 120.0f;
}

float UParkourSystemComponent::GetVerticalWallDetectStartHeight()
{
	static FGameplayTag NotBusyTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.NotBusy"));
	static FGameplayTag ClimbTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.Climb"));

	if(ParkourStateTag.MatchesTag(NotBusyTag))
	{
		return OwnerCharacter->GetActorLocation().Z - (CharacterMovementComponent->IsFalling() ? 40.0f : 70.0f);
	}
	else if(ParkourStateTag.MatchesTag(ClimbTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("UParkourSystemComponent::GetVerticalWallDetectStartHeight should not be here"))
		return 0.0f;
		if(!ArrowActor)
		{
			UE_LOG(LogTemp, Error, TEXT("ArrowActor is NULL"))
			return 0.0f;
		}
	}
	return 0.0f;
}

