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


	//Size

	float WallHeight = 0.0f;
	float WallDepth = 0.0f;
	float VaultHeight = 0.0f;
	float DistanceFromLedgeXY = 9999.0f;

	//Parkour Type

	bool InGround = true;


	UPROPERTY(EditAnywhere)
	TObjectPtr<UParkourVariableDataAsset> CurrentParkourVariables;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> CurrentParkourMontage;

	//Tags
	FGameplayTag StateNotBusyTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.NotBusy"));

	//-----------------Final variables-------------//

	UPROPERTY(EditDefaultsOnly, Category = "Vault | Detect Wall Trace")
	float InitialCapsuleTraceDistance = 150.0f;

	//Determines the number of units of horizontal traces i.e, 2 traces left, 2 traces right

	UPROPERTY(EditDefaultsOnly, Category = "Vault | Detect Wall Trace")
	int32 HorizontalWallDetectTraceHalfQuantity = 2;

	//Determines the distance between each horizontal trace
	UPROPERTY(EditDefaultsOnly, Category = "Vault | Detect Wall Trace")
	float HorizontalWallDetectTraceRange = 20.0f;

	//Determines the number of units of vertical traces go upwards
	UPROPERTY(EditDefaultsOnly, Category = "Vault | Detect Wall Trace")
	int32 VerticalWallDetectTraceQuantity = 30;

	//Determines the distance between each vertical trace
	UPROPERTY(EditDefaultsOnly, Category = "Vault | Detect Wall Trace")
	float VerticalWallDetectTraceRange = 8.0f;

	//Determines the distance between each vertical trace
	UPROPERTY(EditDefaultsOnly, Category = "Vault | Forward Distance Trace")
	int32 ForwardDistanceTraceQuantity = 8;

	//Determines the distance between each vertical trace
	UPROPERTY(EditDefaultsOnly, Category = "Vault | Forward Distance Trace")
	int32 ForwardDistanceTraceBetweenSpace = 30;

	//Determines the distance between each vertical trace
	UPROPERTY(EditDefaultsOnly, Category = "Vault | Find Ground Distance Trace")
	int32 FindForwardGroundDistanceTrace = 70;

	//Location and Shape

	TArray<FHitResult> WallHeightTraces;
	
	FHitResult WallHitResult;

	TArray<FHitResult> PotentialWallHeightTraces;
	
	FRotator WallRotation;
	
	FHitResult WallTopHit;

	FHitResult WallTopResult;

	FVector WarpTopPoint;

	FHitResult WallDepthResult;

	FVector WarpDepth;

	FHitResult WallVaultResult;

	FVector WarpVaultLocation;

	UPROPERTY(EditAnywhere, Category = "Trace|Hit Results")
	bool ShowHitResult = false;

	FGameplayTag CurrentVaultHeightTag;

	FGameplayTag CurrentVaultDepthTag;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UParkourVariableDataAsset>> ParkourVariablesArray;

public:
	//debug lines
	void DrawDebugRotationLines(FRotator InRotation);

	//-------
	
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
	
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	void SetParkourAction(FGameplayTag InNewParourAction);

	void GetInitialCapsuleTraceSettings(FVector& OutStart, FVector& OutEnd, float& Radius, float& OutHalfHeight);

	float GetVerticalWallDetectStartHeight();

	void FindWallDepth();

	bool CheckMantleSurface();

	bool CheckVaultSurface();

	void ShowHitResults();

	void SetParkourState(FGameplayTag InNewParkourState);

	void ParkourStateSettings(ECollisionEnabled::Type InCollisionType, EMovementMode InMode, FRotator InRotationRate, bool bDoCollisionTest, bool bStopMovementImmediately);

	FVector FindWarp1Location(float InWarp1XOffset, float InWarp1ZOffset);

	FVector FindWarp2Location(float InWarp2XOffset, float InWarp2ZOffset);

	FVector FindWarp3Location(float InWarp3XOffset, float InWarp3ZOffset);

	UFUNCTION(BlueprintCallable)
	void StopParkourMontage();

	void ResetParkourResult();

	void FindVaultOrMantleType();

	void PrintVaultHeightandDepth();

};