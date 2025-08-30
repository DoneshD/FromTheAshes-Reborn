#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HitReactionComponent.h"
#include "Components/ActorComponent.h"
#include "CombatStateComponent.generated.h"

class UGameplayEffect;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCombatStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag GroundedTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Grounded");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AirborneTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Airborne");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag NeutralTag = FGameplayTag::RequestGameplayTag("Character.State.Neutral");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DownedTag = FGameplayTag::RequestGameplayTag("Character.State.Downed");

public:

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct StaggerInfo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct SuspendInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct LaunchInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct KnockdownInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct KnockbackInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct SlamInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct DownFlinchInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	FHitReactionStruct BounceInfo;

protected:
	UCombatStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
