#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageInfo.h"
#include "DamageSystem.generated.h"

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
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInvincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInterruptible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBlocking = false;

	ECanBeDamaged DamageOutput;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float Heal(float HealAmount);

	bool TakeDamage(FDamageInfo DamageInfo);

	ECanBeDamaged CanBeDamaged(bool ShouldDamageInvincible, bool CanBeBlocked);

		
};
