#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroupCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UGroupCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	


public:	
	UGroupCombatComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
