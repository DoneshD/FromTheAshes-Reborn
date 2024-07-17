#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FTAEnums.h"
#include "FTACharacter.generated.h"

class UFTAAbilitySystemComponent;
class UFTAAttributeSet;
class UFTAGameplayAbility;
class UGameplayEffect;

UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	
	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFTAAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UFTAGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

public:
	AFTACharacter();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UFTAAttributeSet* GetAttributeSet() const;

	int32 GetAbilityLevel(EGAbilityInputID AbilityID);
	
	void GiveDefaultAbilities();

	void InitDefaultAttributes() const;
};
