#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CookieGameJamGameInstance.generated.h"

UCLASS()
class COOKIEGAMEJAM_API UCookieGameJamGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MusicVolume = 0.7f;
};