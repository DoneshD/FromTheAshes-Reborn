#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ParkourInterface.h"
#include "Components/ActorComponent.h"
#include "ParkourSystemComponent.generated.h"

class UParkourVariableDataAsset;
class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UParkourSystemComponent : public UActorComponent, public IParkourInterface
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|Character")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CharacterMovement")
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|SkeletalMesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|AnimInstance")
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CapsuleComponent")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CameraBoomComponent")
	TObjectPtr<USpringArmComponent> CameraBoomComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CameraComponent")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|MotionWarpingComponent")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWidgetActor> WidgetActorClass;

	//UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AWidgetActor> WidgetActor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArrowActor> ArrowActorClass;

	//UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AArrowActor> ArrowActor;

	float ArrowLocationX;
	float CharacterHeightDiff;
	float ArrowLocationZ = 195;

	float FirstTargetArmLength;
	FVector FirstArmRelativeLocation;

	FGameplayTag ParkourActionTag = FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction");
	FGameplayTag ParkourStateTag = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");

	bool CanManualClimb = false;
	bool CanAutoClimb = true;
	bool MemberAutoClimb = true;


	//Location and Shape

	TArray<FHitResult> WallHitTraces;
	
	FHitResult WallHitResult;

	TArray<FHitResult> HopHitTraces;
	
	FRotator WallRotation;
	
	FHitResult TopHits;

	FHitResult WallTopResult;

	FVector WarpTopPoint;
	
	int32 HorizontalWallDetectTraceHalfQuantity = 2;

	float HorizontalWallDetectTraceRange = 20.0f;

	int32 VerticalWallDetectTraceQuantity = 30;

	float VerticalWallDetectTraceRange = 8.0f;

	FHitResult WallDepthResult;

	FVector WarpDepth;

	FHitResult WallVaultResult;

	FVector WarpVaultLocation;

	UPROPERTY(EditAnywhere, Category = "Trace|Hit Results")
	bool ShowHitResult = false;

	//Size

	float WallHeight = 0.0f;
	float WallDepth = 0.0f;
	float VaultHeight = 0.0f;
	float DistanceFromLedgeXY = 9999.0f;

	//Parkour Type

	bool InGround = true;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UParkourVariableDataAsset>> ParkourVariablesArray;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParkourVariableDataAsset> CurrentParkourVariables;

public:
	UParkourSystemComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ParkourInputPressedVault();

	virtual bool SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom, UCameraComponent* Camera,
		UMotionWarpingComponent* MotionWarping) override;
	
	bool SelectClimb(bool InCanManualClimb, bool InCanAutoClimb, bool InAutoClimb);

	void ParkourAction(bool InAutoClimb);

	void FindParkourLocationAndShape();

	void FindSizeParkourObjects();

	void FindParkourType(bool AutoClimb);

	void PlayParkourMontage();
	
	void SetParkourAction(FGameplayTag InNewParourAction);

	void GetFirstCapsuleTraceSettings(FVector& OutStart, FVector& OutEnd, float& Radius, float& OutHalfHeight);

	float GetVerticalWallDetectStartHeight();

	void FindEdgeofWall();

	bool CheckMantleSurface();

	bool CheckVaultSurface();

	void ShowHitResults();

	void SetParkourState(FGameplayTag InNewParkourState);

	void ParkourStateSettings();

};