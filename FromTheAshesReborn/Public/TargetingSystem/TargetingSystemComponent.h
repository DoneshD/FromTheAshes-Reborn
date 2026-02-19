#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystemComponent.generated.h"

class UCameraParamsDataAsset;
class UCameraSystemComponent;
class AFTAPlayerCameraManger;
class APlayerCharacter;
class UUserWidget;
class UWidgetComponent;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceComponentOnTargetLockedOnOff, AActor*, TargetActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceComponentSetRotation, AActor*, TargetActor, FRotator, ControlRotation);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float MinimumDistanceToEnable = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Offset")
	float MaxDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Offset")
	float MinDistance = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Target System Delegates")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOff;

	UPROPERTY(BlueprintAssignable, Category = "Target System Delegates")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraParamsDataAsset> CameraParameters;
	
protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TSubclassOf<AActor> TargetableActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TEnumAsByte<ECollisionChannel> TargetableCollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float BreakLineOfSightDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	bool ShouldDrawLockedOnWidget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> LockedOnWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	float LockedOnWidgetDrawSize = 32.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName LockedOnWidgetParentSocket = FName("spine_03");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FVector LockedOnWidgetRelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	
	UPROPERTY(BlueprintAssignable, Category = "Target System Delegates")
	FTraceComponentSetRotation OnTargetSetRotation;

	bool IgnoreLookInput = false;

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	APlayerController* OwnerPlayerController;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<AFTAPlayerCameraManger> FTAPlayerCameraManger;

	UPROPERTY()
	TObjectPtr<UCameraSystemComponent> CameraSystemComponent;

	UPROPERTY()
	UWidgetComponent* TargetLockedOnWidgetComponent;

	UPROPERTY()
	AActor* LockedOnTargetActor;

	FTimerHandle LineOfSightBreakTimerHandle;
	FTimerHandle SwitchingTargetTimerHandle;

	bool IsBreakingLineOfSight = false;


protected:

	UTargetingSystemComponent();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DrawCameraAnchor();

	void TargetLockOn(AActor* TargetToLockOn);
	void CreateAndAttachTargetLockedOnWidgetComponent(AActor* TargetActor);
	void SetupLocalPlayerController();
	static bool TargetIsTargetable(const AActor* Actor);

	void SetOwnerActorRotation();
	void EnableControlRotation(bool ShouldControlRotation) const;

	FVector CalculateAnchorLocation(APlayerCharacter* PlayerOwner, const AActor* TargetActor, float DeltaTime, TObjectPtr<UCameraParamsDataAsset> CameraParams);
	float CalculateBaseSpringArmLength(APlayerCharacter* PlayerOwner, const AActor* TargetActor);
	// void SetControlRotationOnTarget(AActor* TargetActor) const;
	
	TArray<AActor*> FindTargetsInRange(TArray<AActor*> ActorsToLook, float RangeMin, float RangeMax) const;
	
	AActor* FindNearestTargetToCenterViewport(TArray<AActor*> Actors) const;

	bool LineTrace(FHitResult& OutHitResult, const AActor* OtherActor, const TArray<AActor*>& ActorsToIgnore) const;
	bool LineTraceForActor(const AActor* OtherActor, const TArray<AActor*>& ActorsToIgnore) const;

	bool ShouldBreakLineOfSight() const;
	void BreakLineOfSight();

	float FindDistanceFromCenterOfViewport(const AActor* TargetActor) const;

	float GetAngleUsingCameraRotation(const AActor* ActorToLook) const;
	float GetAngleUsingCharacterRotation(const AActor* ActorToLook) const;
	static FRotator FindLookAtRotation(const FVector Start, const FVector Target);

public:

	FVector CurrentAnchorLocation = FVector::ZeroVector;
	FVector TargetCameraAnchorLocation = FVector::ZeroVector;

	float TargetSpringArmLength = 0.0f;
	
	FRotator TargetCameraAnchorRotation = FRotator::ZeroRotator;
	bool UseWorldTransform = true;
	float CameraAnchorInterpSpeed;

	//GA_LockOn entry point
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* TargetActor(bool& IsSuccess);
	
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* GetLockedOnTargetActor() const;

	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool IsLocked() const;
	
	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool GetTargetLockedStatus();
	
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetLockOff();

	TArray<AActor*> GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const;
	AActor* FindNearestTargetToActor(TArray<AActor*> Actors) const;

	float ClosestTargetDistance = 0.0f;

	bool IsTargetLocked = false;
	bool bIsLockingOn = false;
	
};
