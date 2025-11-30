#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AfterImageComponent.generated.h"


class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UAfterImageComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;

	UFUNCTION(BlueprintCallable)
	void CreateAfterImage();

protected:
	
	UAfterImageComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
