#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GarryItemRow.generated.h"

USTRUCT(BlueprintType)
struct FGarryItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AActor> ObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Reward = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 100.f; //Larger number means more common
};