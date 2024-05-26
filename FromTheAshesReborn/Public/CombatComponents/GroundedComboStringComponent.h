#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroundedComboStringComponent.generated.h"

class APlayableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UGroundedComboStringComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<APlayableCharacter> PC;

	FString CurrentComboString = TEXT("");

	TArray<FString> ComboSequences;

	TArray<TObjectPtr<UAnimMontage>> CurrentComboSequence;

	TMap<FString, TArray<TObjectPtr<UAnimMontage>>> ComboStringMap;

	FString Combo1 = TEXT("LLLLLLL");
	FString Combo2 = TEXT("HHHHH");
	FString Combo3 = TEXT("LLHLH");
	FString Combo4 = TEXT("HHLLH");

	bool IsLightAttackSaved = false;
	bool IsHeavyAttackSaved = false;

	int CurrentAttackIndex = 0;

	int CurrentLightAttackIndex = 0;
	int CurrentHeavyAttackIndex = 0;


protected:

public:	

	UGroundedComboStringComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SelectComboString();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SaveLightAttack();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SaveHeavyAttack();

	void LightAttack();

	void HeavyAttack();

	

	void PerformCurrentAttack(float WarpingDistance, TObjectPtr<UAnimMontage> CurrentAttackMontage);

	void PerformLightAttack(int LightAttackIndex);
		
	void PrintCurrentComboString();

};
