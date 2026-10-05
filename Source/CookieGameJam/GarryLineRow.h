#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GarryLineRow.generated.h"

UENUM(BlueprintType)
enum class EGarryLineType : uint8
{
	Request,
	Reminder,
	Complete,
	Timeout,
	Idle
	//Add more types if I want later such as upgrade purchase
};

USTRUCT(BlueprintType)
struct FGarryLineRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGarryLineType Type = EGarryLineType::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Line;
};