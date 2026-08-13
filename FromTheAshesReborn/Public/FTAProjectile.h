#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "FTAProjectile.generated.h"

class UMoveToLocationDataAsset;
class UGA_ReceiveHit;
class URangedAbilityDataAsset;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class FROMTHEASHESREBORN_API AFTAProjectile : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Core Components")
	TObjectPtr<USphereComponent> CollisionComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Core Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Core Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<URangedAbilityDataAsset> RangedAbilityDataAsset;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilityTargetDataHandle TargetHitDataHandle;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> SourceActor;

	FGameplayEventData OnHitEventData;

	UPROPERTY()
	TObjectPtr<UMoveToLocationDataAsset> CurrentMoveToLocationAsset;

protected:
	
	AFTAProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//TODO: Temp, this is weird
	UFUNCTION()
	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);
	
	void ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	void GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	
	void ApplyHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	void AddHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);

	UFUNCTION(BlueprintCallable)
	void SendHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitDataInfo& HitData);

};
