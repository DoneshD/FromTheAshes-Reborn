#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AfterImageActor.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AAfterImageActor : public AActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USkeletalMeshComponent* MeshToCopy = nullptr;
	

public:
	AAfterImageActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnAfterImageActor();
	
};
