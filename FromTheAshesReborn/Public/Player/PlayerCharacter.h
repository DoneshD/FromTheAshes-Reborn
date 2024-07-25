#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "PlayerCharacter.generated.h"

struct FGameplayEffectSpec;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayerCharacter : public AFTACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;

public:
	APlayerCharacter();

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	void InitAbilitySystemComponent();

	void AddCharacterAbilities();
	
};
