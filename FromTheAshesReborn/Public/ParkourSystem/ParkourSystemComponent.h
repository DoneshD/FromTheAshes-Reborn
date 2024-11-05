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
	TSubclassOf<class AArrowActor> ArrowActorClass;
	
	TObjectPtr<class AArrowActor> ArrowActor;

	float ArrowLocationX;
	float CharacterHeightDiff;
	float ArrowLocationZ = 195;

	float FirstTargetArmLength;
	FVector FirstArmRelativeLocation;

	FGameplayTag ParkourActionTag = FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction");
	FGameplayTag ParkourStateTag = FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy");
	
	//Auto Climb
	bool CanManualClimb = false;
	bool CanAutoClimb = true;
	bool MemberAutoClimb = true;

	//Distance results
	float WallHeight = 0.0f;
	float WallDepth = 0.0f;
	float VaultHeight = 0.0f;
	float DistanceFromLedgeXY = 9999.0f;

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
	TArray<FHitResult> PotentialWallHeightTraces;
	
	FRotator WallRotation;
	
	FHitResult WallHitResult;
	FHitResult WallTopHit;
	FHitResult WallDepthResult;
	FHitResult WallTopResult;
	FHitResult WallVaultResult;

	FVector WarpTopPoint;
	FVector WarpDepth;
	FVector WarpVaultLocation;

	UPROPERTY(EditAnywhere, Category = "Trace|Hit Results")
	bool ShowHitResult = false;

	FGameplayTag CurrentVaultHeightTag;
	FGameplayTag CurrentVaultDepthTag;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UParkourVariableDataAsset>> ParkourVariablesArray;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParkourVariableDataAsset> CurrentParkourVariables;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> CurrentParkourMontage;

public:
	
	UParkourSystemComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void ParkourInputPressedVault();

	UFUNCTION()
	virtual bool SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom, UCameraComponent* Camera,
		UMotionWarpingComponent* MotionWarping) override;

	UFUNCTION()
	bool SelectClimb(bool InCanManualClimb, bool InCanAutoClimb, bool InAutoClimb);

	UFUNCTION()
	void GetInitialCapsuleTraceSettings(FVector& OutStart, FVector& OutEnd, float& Radius, float& OutHalfHeight);

	UFUNCTION()
	float GetVerticalWallDetectStartHeight();

	UFUNCTION()
	void SelectParkourAction(bool InAutoClimb);

	UFUNCTION()
	void FindParkourLocationAndShape();

	UFUNCTION()
	void FindWallDepth();
	
	UFUNCTION()
	void FindSizeParkourObjects();

	UFUNCTION()
	void FindParkourType(bool AutoClimb);

	UFUNCTION()
	void FindVaultOrMantleType();
	
	UFUNCTION()
	bool CheckMantleSurface();

	UFUNCTION()
	bool CheckVaultSurface();

	UFUNCTION()
	void SetParkourAction(FGameplayTag InNewParourAction);
	
	UFUNCTION()
	void PlayParkourMontage();

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void SetParkourState(FGameplayTag InNewParkourState);

	UFUNCTION()
	void ParkourStateSettings(ECollisionEnabled::Type InCollisionType, EMovementMode InMode, FRotator InRotationRate, bool bDoCollisionTest, bool bStopMovementImmediately);

	UFUNCTION()
	FVector FindWarp1Location(float InWarp1XOffset, float InWarp1ZOffset);
	
	UFUNCTION()
	FVector FindWarp2Location(float InWarp2XOffset, float InWarp2ZOffset);
	
	UFUNCTION()
	FVector FindWarp3Location(float InWarp3XOffset, float InWarp3ZOffset);

	//---------Reset----------
	
	UFUNCTION()
	void ResetParkourResult();

	//----------------------Show results-----------------//
	
	UFUNCTION()
	void ShowHitResults();
	
	UFUNCTION()
	void PrintVaultHeightandDepth();

	void DrawDebugRotationLines(FRotator InRotation);
};