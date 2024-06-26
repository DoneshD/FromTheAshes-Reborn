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

	TArray<TObjectPtr<UAnimMontage>> CurrentComboSeqAnim;

	TMap<FString, TArray<TObjectPtr<UAnimMontage>>> ComboStringMap;

	FString Combo1 = TEXT("LLLLLLL");
	FString Combo2 = TEXT("HHHHH");
	FString Combo3 = TEXT("LLHLH");
	FString Combo4 = TEXT("HHLLH");
	FString Combo5 = TEXT("LHLHHL");
	FString Combo6 = TEXT("HHHL");
	FString Combo7 = TEXT("LLPLLLLL");
	FString Combo8 = TEXT("HPHHH");
	FString Combo9 = TEXT("LLLPHH");
	FString Combo10 = TEXT("HHPLHL");

	bool IsLightAttackSaved = false;
	bool IsHeavyAttackSaved = false;
	bool IsAttackPaused = false;

	int CurrentAttackIndex = 0;

	FTimerHandle FAttackPauseHandle;

protected:

public:	

	UGroundedComboStringComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ResetComboString();

	void SelectComboString();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SaveLightAttack();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SaveHeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void StartAttackPauseTimer();

	void ClearAttackPauseTimer();

	void AppendAttackPause();

	void AppendLightAttack();

	void AppendHeavyAttack();

	void PerformCurrentAttack(int LightAttackIndex);
		

};
