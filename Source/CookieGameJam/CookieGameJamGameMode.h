// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DayRow.h"
#include "Engine/DataTable.h"
#include "CookieGameJamGameMode.generated.h"

USTRUCT(BlueprintType)
struct FDayResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bPassed = false;

	UPROPERTY(BlueprintReadOnly)
	FText Title;

	UPROPERTY(BlueprintReadOnly)
	int32 JobsCompleted = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 JobsRequired = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 JobsOverQuota = 0;

	UPROPERTY(BlueprintReadOnly)
	float TimeLimit = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float TimeTaken = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bHasNextLevel = false;
};

UCLASS(abstract)
class ACookieGameJamGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACookieGameJamGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Days")
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Days")
	FName DayRowOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Days")
	FName MainMenuLevel = FName("MainMenu");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Days")
	bool bEndDayEarlyOnQuota = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Days")
	bool bPauseOnDayEnd = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Days")
	FDayRow CurrentDayRow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Days")
	int32 JobsCompleted = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Days")
	bool bDayActive = false;

	UFUNCTION(BlueprintCallable, Category = "Days")
	void OnJobCompleted();

	UFUNCTION(BlueprintCallable, Category = "Days")
	float GetTimeRemaining() const;

	UFUNCTION(BlueprintCallable, Category = "Days")
	void RestartDay();

	UFUNCTION(BlueprintCallable, Category = "Days")
	bool LoadNextDay();

	UFUNCTION(BlueprintCallable, Category = "Days")
	void GoToMainMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "Days")
	void OnDayStarted(const FDayRow& DayRow);

	UFUNCTION(BlueprintImplementableEvent, Category = "Days")
	void OnJobProgress(int32 Completed, int32 Required);

	UFUNCTION(BlueprintImplementableEvent, Category = "Days")
	void OnDayEnded(const FDayResult& Result);

protected:
	virtual void BeginPlay() override;

private:
	void StartDay();
	void FinishDay();
	void PrepareForLevelChange();

	FTimerHandle DayTimerHandle;
};