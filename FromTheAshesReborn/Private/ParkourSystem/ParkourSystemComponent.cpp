#include "ParkourSystem/ParkourSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
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
		if(SelectClimb(CanManualClimb, CanAutoClimb, MemberAutoClimb))
		{
			FindParkourLocationAndShape();
			ShowHitResults();
			FindSizeParkourObjects();
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
			FindParkourType(MemberAutoClimb);

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
		ActorsToIgnore, EDrawDebugTrace::None, FirstCapsuleTraceOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

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
		if(WallHitResult.bBlockingHit && !WallHitResult.bStartPenetrating)
		{
			if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")))
			{
				
			}
			else
			{
				WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
			}
			for(int32 m = 0; m <= 8; m++)
			{

				FVector ImpactPointForwardVector = WallHitResult.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * (m * 30)) + (UKismetMathLibrary::GetForwardVector(WallRotation) * 2.0f);
				FVector SphereTraceStartLocation = ImpactPointForwardVector + FVector(0.0f, 0.0f, 7.0f);
				FVector SphereTraceEndLocation = SphereTraceStartLocation - FVector(0.0f, 0.0f, 7.0f);

				FHitResult FSphereTraceOutHitResult;
				TArray<AActor*> ActorsToIgnoreSphere;

				bool bSphereTraceOutHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStartLocation, SphereTraceEndLocation, 10.5f, TraceTypeQuery1,
					false, ActorsToIgnore, EDrawDebugTrace::None, FSphereTraceOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

				if(m == 0)
				{
					if(bSphereTraceOutHit)
					{
						WallTopResult = FSphereTraceOutHitResult;
						WarpTopPoint = FSphereTraceOutHitResult.ImpactPoint;
					}
				}
				
				if(bSphereTraceOutHit)
				{
					TopHits = FSphereTraceOutHitResult;
					if(m == 8)
					{
						FindEdgeofWall();
					}
				}
				else
				{
					break;
				}
			}
			
			FindEdgeofWall();
		}
	}
}

void UParkourSystemComponent::FindSizeParkourObjects()
{
	if(WallHitResult.bBlockingHit)
	{
		if(WallTopResult.bBlockingHit)
		{
			if(OwnerCharacter->GetMesh())
			{
				WallHeight = WallTopResult.ImpactPoint.Z - OwnerCharacter->GetMesh()->GetSocketLocation(FName("root")).Z;
				UE_LOG(LogTemp, Warning, TEXT("WallHeight: %f"), WallHeight);
			}
		}
		else
		{
			WallHeight = 0.0f;
		}

		if(WallTopResult.bBlockingHit && WallDepthResult.bBlockingHit)
		{
			WallDepth = FVector::Dist(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint);
			UE_LOG(LogTemp, Warning, TEXT("WallDepth: %f"), WallDepth);

		}
		else
		{
			WallDepth = 0.0f;
		}

		if(WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit)
		{
			VaultHeight = WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z;
			UE_LOG(LogTemp, Warning, TEXT("VaultHeight: %f"), VaultHeight);

		}
		else
		{
			VaultHeight = 999.0f;
		}

		if(WallTopResult.bBlockingHit)
		{
			DistanceFromLedgeXY = FVector::Dist(OwnerCharacter->GetActorLocation(), WallTopResult.ImpactPoint);
		}
		else
		{
			DistanceFromLedgeXY = 9999.0f;
		}
	}
	else
	{
		WallHeight = 0.0f;
		WallDepth = 0.0f;
		VaultHeight = 0.0f;
		DistanceFromLedgeXY = 9999.0f;
	}
}

void UParkourSystemComponent::FindParkourType(bool InAutoClimb)
{
	if(WallTopResult.bBlockingHit)
	{
		if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy")))
		{
			if(InGround)
			{
				if(UKismetMathLibrary::InRange_FloatFloat(WallHeight, 90.0f, 120.0f, false, true))
				{
					if(CheckMantleSurface())
					{
						if(UKismetMathLibrary::InRange_FloatFloat(WallDepth, 0.0f, 120.0f, true, true))
						{
							if(UKismetMathLibrary::InRange_FloatFloat(VaultHeight, 60.0f, 120.0f, true, true))
							{
								if(UKismetMathLibrary::InRange_FloatFloat(WallDepth, 0.0f, 30.0f, true, true))
								{
									SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.ThinVault"));
								}
								else
								{
									if(CharacterMovementComponent->Velocity.Length() > 20.0f)
									{
										SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Vault"));
									}
									else
									{
										SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.Mantle"));

									}
								}
							}
							else
							{
								SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.State.Mantle"));
							}
						}
						else
						{
							SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.State.Mantle"));
						}
					}
					else
					{
						SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
					}
				}
			}
		}
		else if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")))
		{
			
		}
	}
	else
	{
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		if(!InAutoClimb)
		{
			OwnerCharacter->Jump();
		}
	}
}

void UParkourSystemComponent::SetParkourAction(FGameplayTag InNewParourAction)
{
	
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

void UParkourSystemComponent::FindEdgeofWall()
{
	if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")) || ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy")))
	{
		
		FVector WallEdgeStartLocation = TopHits.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * 30.0f);
		FVector WallEdgeEndLocation = TopHits.ImpactPoint;
		
		FHitResult FWallEdgeOutHitResult;
		TArray<AActor*> ActorsToIgnoreWallEdge;
		
		bool bWallEdgeTraceOutHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WallEdgeStartLocation, WallEdgeEndLocation, 10.0f, TraceTypeQuery1,
					false, ActorsToIgnoreWallEdge, EDrawDebugTrace::None, FWallEdgeOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

		if(bWallEdgeTraceOutHit)
		{
			WallDepthResult = FWallEdgeOutHitResult;
			WarpDepth = WallDepthResult.ImpactPoint;

			FVector WallGroundStartLocation = WallDepthResult.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * 70.0f);
			FVector WallGroundEndLocation = WallGroundStartLocation - FVector(0.0f, 0.0f, 400.0f);
		
			FHitResult FWallGroundOutHitResult;
			TArray<AActor*> ActorsToIgnoreWallGround;
		
			bool bWallGroundTraceOutHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WallGroundStartLocation, WallGroundEndLocation, 30.0f, TraceTypeQuery1,
						false, ActorsToIgnoreWallGround, EDrawDebugTrace::None, FWallGroundOutHitResult, true, FLinearColor::Blue, FLinearColor::Yellow, 5.0f);

			if(bWallGroundTraceOutHit)
			{
				WallVaultResult = FWallGroundOutHitResult;
				WarpVaultLocation = WallVaultResult.ImpactPoint;
			}
		}
	}
}

bool UParkourSystemComponent::CheckMantleSurface()
{
	//WallTopResult.ImpactPoint
	FHitResult CapsuleTraceOutHitResult;
	TArray<AActor*> ActorsToIgnore; 
	FVector CapsuleStartLocation = WarpTopPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() + 8.0f);
	FVector CapsuleEndLocation = WarpTopPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() + 8.0f);

	bool bCapsuleTraceOutHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CapsuleStartLocation, CapsuleEndLocation, 25.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() - 8.0f, TraceTypeQuery1, false,
	ActorsToIgnore, EDrawDebugTrace::ForDuration, CapsuleTraceOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	return !bCapsuleTraceOutHit;
}

void UParkourSystemComponent::ShowHitResults()
{
	if(ShowHitResult)
	{
		if(WallTopResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallTopResult.ImpactPoint, 10.0f, 12, FColor::Blue, false, 5.0f);
		}
		if(WallDepthResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallDepthResult.ImpactPoint, 10.0f, 12, FColor::Orange, false, 5.0f);
		}
		if(WallVaultResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallVaultResult.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 5.0f);
		}
	}
}

