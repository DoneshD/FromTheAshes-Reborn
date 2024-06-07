#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageInfo.h"
#include "Enums/EHitReactionDirection.h"
#include "Enums/EHitDirection.h"
#include "DamageSystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageResponse, FDamageInfo)
DECLARE_DELEGATE(FOnDeathResponse)

UENUM(BlueprintType)
enum class ECanBeDamaged : uint8
{
	ECanBeDamaged_BlockedDamage UMETA(DisplayName = "BlockedDamage"),
	ECanBeDamaged_DoDamage		UMETA(DisplayName = "DoDamage"),
	ECanBeDamaged_NoDamage		UMETA(DisplayName = "NoDamage"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDamageSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInvincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInterruptible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBlocking = false;

	ECanBeDamaged DamageOutput;

	FOnDamageResponse OnDamageResponse;
	FOnDeathResponse OnDeathResponse;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> LeftHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> RightHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> FrontHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> BackHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> KnockbackHitReaction;


protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	ECanBeDamaged CanBeDamaged(bool ShouldDamageInvincible, bool CanBeBlocked);

	float Heal(float HealAmount);

	bool TakeDamage(FDamageInfo DamageInfo);

	void HandleHitReaction(FDamageInfo DamageInfo);
		
};
