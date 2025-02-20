#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "Player/PlayerComboManagerInterface.h"
#include "EnemyBaseCharacter.generated.h"

class UPlayerComboManagerComponent;
class UBehaviorTree;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter, public IPlayerComboManagerInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "PlayerComboManager")
	TObjectPtr<UPlayerComboManagerComponent> PlayerComboManagerComponent;

protected:
	
	AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FGameplayTagContainer& GetCurrentComboContainer() override;

	virtual int GetCurrentComboIndex() override;

	virtual void SetCurrentComboIndex(int ComboIndex) override;

};
