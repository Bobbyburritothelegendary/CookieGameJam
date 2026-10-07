#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DayRow.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FDayRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 JobsRequired = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLimit = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundBase*> Music;
};