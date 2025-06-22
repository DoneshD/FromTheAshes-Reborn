#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "FTAPlayerState.generated.h"

class UFTAAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFTAOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS()
class FROMTHEASHESREBORN_API AFTAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "PlayerState | ASComponent")
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<AActor> HardLockedTargetActor = nullptr;
	
public:
	
	AFTAPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "PlayerState | ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const { return FTAAbilitySystemComponent; }
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

};
