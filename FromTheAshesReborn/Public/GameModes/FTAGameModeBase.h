#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FTAGameModeBase.generated.h"

class AEnemyBaseCharacter;

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Testing")
	TObjectPtr<UAudioComponent> CombatMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Testing")
	int32 InitialEnemyCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Testing")
	bool PlayMusic = false;
	
public:

	AFTAGameModeBase();
	virtual void BeginPlay() override;
	
};
