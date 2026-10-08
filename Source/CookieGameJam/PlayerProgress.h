#pragma once

#include "CoreMinimal.h"
#include "PlayerProgress.generated.h"

USTRUCT(BlueprintType)
struct FPlayerProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cash = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PayRiseLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimeLevel = 1;
};