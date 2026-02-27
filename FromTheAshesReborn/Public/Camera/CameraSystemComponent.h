#pragma once

#include "CoreMinimal.h"
#include "CameraParamsDataAsset.h"
#include "Components/ActorComponent.h"
#include "CameraSystemComponent.generated.h"

class UCameraParamsDataAsset;
class UTargetingSystemComponent;
class AFTAPlayerCameraManger;

class USpringArmComponent;
class UCameraComponent;
class APlayerCharacter;

UCLASS(BlueprintType, Const)
class FROMTHEASHESREBORN_API UCameraSystemComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	TObjectPtr<AFTAPlayerCameraManger> FTAPlayerCameraManger;

	UPROPERTY()
	TArray<TObjectPtr<UCameraParamsDataAsset>> CameraParamsArray;

protected:

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY()
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY()
	TObjectPtr<USceneComponent> CameraAnchorComponent;

	UPROPERTY()
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;
	
	FRotator CurrentCameraOffset;

	//Temp
	float CameraBaseFOV = 90.0f;
	float CameraFOVOffset = 0.0f;
	float CameraFOVLerpSpeed = 2.0f;

public:
	
	UPROPERTY()
	APlayerController* OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UCameraParamsDataAsset> NeutralCameraStateParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UCameraParamsDataAsset> CurrentCameraStateParams;

	bool UseControllerRotation = true;

public:
	
	template<typename TValue, typename TParam>
	struct FCameraParamAccessors
	{
		TFunction<const TParam&(const UCameraParamsDataAsset*)> GetParam;
		TFunction<const FCameraValueData&(const TParam&)> GetValueMetaData;
		TFunction<const TValue&(const TParam&)> GetValue;
		TFunction<void(TValue&, const TValue&)> SetValue;
		TFunction<TValue(const TValue&, const TValue&)> AdditiveOp;
		TFunction<float(const FCameraValueData&)> GetInLerp;
	};
	
	template<typename TValue, typename TParam>
	static void ResolveCameraParam(
	const TArray<TObjectPtr<UCameraParamsDataAsset>>& ParamsArray,
	TValue& TargetValue,
	float& TargetLerpSpeed, 
	const FCameraParamAccessors<TValue, TParam>& Access,
	TFunction<void(TValue&, const TParam&, const UCameraParamsDataAsset*)> SpecialHandler = nullptr)
	{
		if (SpecialHandler)
		{
			for (const auto& Params : ParamsArray)
			{
				const TParam& Param = Access.GetParam(Params.Get());
				SpecialHandler(TargetValue, Param, Params.Get());
			}
		}

		TArray<TObjectPtr<UCameraParamsDataAsset>> Sorted = ParamsArray;

		Sorted.Sort([&](const TObjectPtr<UCameraParamsDataAsset>& A,
						const TObjectPtr<UCameraParamsDataAsset>& B)
		{
			return Access.GetValueMetaData(Access.GetParam(A.Get())).Priority >
				   Access.GetValueMetaData(Access.GetParam(B.Get())).Priority;
		});

		for (const auto& Params : Sorted)
		{
			const TParam& Param = Access.GetParam(Params.Get());
			const FCameraValueData& Meta = Access.GetValueMetaData(Param);

			if (!Meta.ShouldAdjust) continue;

			TargetLerpSpeed = Access.GetInLerp(Meta);
			break;
		}

		for (const auto& Params : Sorted)
		{
			const TParam& Param = Access.GetParam(Params.Get());
			const FCameraValueData& MetaData = Access.GetValueMetaData(Param);

			if (!MetaData.ShouldAdjust) continue;

			if (MetaData.CameraOperation == ECameraOperation::Set)
			{
				Access.SetValue(TargetValue, Access.GetValue(Param));
				break;
			}
		}

		for (const auto& Params : Sorted)
		{
			const TParam& Param = Access.GetParam(Params.Get());
			const FCameraValueData& Meta = Access.GetValueMetaData(Param);

			if (!Meta.ShouldAdjust) continue;

			if (Meta.CameraOperation == ECameraOperation::Additive)
			{
				TargetValue = Access.AdditiveOp(TargetValue, Access.GetValue(Param));
			}
		}
	}

	
	UCameraSystemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HandleSpringArmParams(float DeltaTime);
	
	void SetupLocalPlayerController();

	void ResolveCameraValue(TArray<TObjectPtr<UCameraParamsDataAsset>> SortedArray);
	
	void ResolveSpringArmParams();
	void ResolveControlRotationParams();
	void ResolveCameraAnchorParams();

	void ResolveCameraStateParams();
	
	
	UFUNCTION(BlueprintCallable)
	void AddCameraParameters(UCameraParamsDataAsset* CameraParams);

	UFUNCTION(BlueprintCallable)
	void RemoveCameraParameters(UCameraParamsDataAsset* CameraParams);
	
	float CatchupToOffScreen(const FVector& PlayerLocation, float& InInterpSpeed, TObjectPtr<UCameraParamsDataAsset> CameraParams);
	float CompareDistanceToScreenAndGetInterpSpeed(APlayerCharacter* PlayerOwner, const AActor* TargetActor, bool& InShouldUpdateControlRotation);
	float GetWorldDistanceFromCamera(APlayerController* PlayerController, const AActor* TargetActor);
	
};
