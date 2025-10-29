#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "EnemyBaseCharacter.generated.h"

class UWeaponDefinition;
class UWidgetComponent;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UWidgetComponent> HealthWidget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UNiagaraComponent> DissolveNiagaraComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UStaticMeshComponent> DissolveMeshComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInstance> CodeDissolveMaterial1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInstanceDynamic> CodeDynamicDissolveMaterial1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInstance> CodeDissolveMaterial2;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInstanceDynamic> CodeDynamicDissolveMaterial2;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInstance> CodeDissolveMaterial3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInstanceDynamic> CodeDynamicDissolveMaterial3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UNiagaraSystem> CodeSetDissolveNiagara;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	float CodeDissolveAmount = 0.88f;

	// UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	// class UTexture* CodeDissolveTexture;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	TObjectPtr<UTexture> CodeDissolveTexture;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	FVector2D CodeDissolveTextureUV = FVector2D(1.0f, 1.0f);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	float CodeDissolveStart = 1.1f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dissolve")
	float CodeDissolveEnd = 0.0f;

public:

	UPROPERTY()
	FTimeline DissolveTimeline;

	UPROPERTY(EditAnywhere, Category="Timeline")
	UCurveFloat* FloatCurve;

	UFUNCTION()
	void TimelineProgress(float Value);

	UFUNCTION()
	void TimelineFinished();

	bool ShouldDissolveTimelineTick = false;
	
	bool ShouldRotate = true;
	

protected:
	
	AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator);

	UFUNCTION(BlueprintCallable)
	void CheckDeath(float NewValue);

	void Death();

	UFUNCTION(BlueprintCallable)
	void SetDissolveParams();
	
	void Dissolve();

};
