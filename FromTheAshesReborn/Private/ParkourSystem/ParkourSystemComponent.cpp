#include "ParkourSystem/ParkourSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	
	CapsuleRadius = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	JumpZVelocity = CharacterMovementComponent->JumpZVelocity;

}


void UParkourSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UParkourSystemComponent::ParkourInputPressedVault()
{
	
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


