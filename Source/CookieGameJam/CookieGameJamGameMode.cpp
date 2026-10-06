// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookieGameJamGameMode.h"

#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ACookieGameJamGameMode::ACookieGameJamGameMode()
{
}

void ACookieGameJamGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartDay();
}

void ACookieGameJamGameMode::StartDay()
{
	GetWorldTimerManager().ClearTimer(DayTimerHandle);

	if (!DataTable)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, TEXT("Day table not assigned on the GameMode."));
		}
		return;
	}

	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
	const FName RowName = DayRowOverride.IsNone() ? FName(*LevelName) : DayRowOverride;

	const FDayRow* Row = DataTable->FindRow<FDayRow>(RowName, TEXT("StartDay"));
	if (!Row)
	{
		FString Names;
		for (const FName& Name : DataTable->GetRowNames())
		{
			Names += TEXT("'") + Name.ToString() + TEXT("' ");
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("No day row named '%s'. Rows in table: %s"), *RowName.ToString(), *Names));
		}
		return;
	}

	CurrentDayRow = *Row;
	JobsCompleted = 0;
	bDayActive = true;

	GetWorldTimerManager().SetTimer(DayTimerHandle, this, &ACookieGameJamGameMode::FinishDay, FMath::Max(CurrentDayRow.TimeLimit, 1.f), false);

	OnDayStarted(CurrentDayRow);
	OnJobProgress(JobsCompleted, CurrentDayRow.JobsRequired);
}

void ACookieGameJamGameMode::FinishDay()
{
	if (!bDayActive) return;

	const float Remaining = GetTimeRemaining();
	GetWorldTimerManager().ClearTimer(DayTimerHandle);
	bDayActive = false;

	FDayResult Result;
	Result.Title = CurrentDayRow.Title;
	Result.JobsCompleted = JobsCompleted;
	Result.JobsRequired = CurrentDayRow.JobsRequired;
	Result.bPassed = JobsCompleted >= CurrentDayRow.JobsRequired;
	Result.JobsOverQuota = FMath::Max(0, JobsCompleted - CurrentDayRow.JobsRequired);
	Result.TimeLimit = CurrentDayRow.TimeLimit;
	Result.TimeTaken = FMath::Clamp(CurrentDayRow.TimeLimit - Remaining, 0.f, CurrentDayRow.TimeLimit);
	Result.bHasNextLevel = !CurrentDayRow.NextLevel.IsNone();

	if (bPauseOnDayEnd)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
		}
		UGameplayStatics::SetGamePaused(this, true);
	}

	OnDayEnded(Result);
}

void ACookieGameJamGameMode::OnJobCompleted()
{
	if (!bDayActive) return;

	JobsCompleted++;
	OnJobProgress(JobsCompleted, CurrentDayRow.JobsRequired);

	if (bEndDayEarlyOnQuota && JobsCompleted >= CurrentDayRow.JobsRequired)
	{
		FinishDay();
	}
}

float ACookieGameJamGameMode::GetTimeRemaining() const
{
	const FTimerManager& TM = GetWorldTimerManager();
	if (TM.IsTimerActive(DayTimerHandle))
	{
		return FMath::Max(TM.GetTimerRemaining(DayTimerHandle), 0.f);
	}

	return 0.f;
}

void ACookieGameJamGameMode::PrepareForLevelChange()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void ACookieGameJamGameMode::RestartDay()
{
	PrepareForLevelChange();
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

bool ACookieGameJamGameMode::LoadNextDay()
{
	if (CurrentDayRow.NextLevel.IsNone()) return false;

	PrepareForLevelChange();
	UGameplayStatics::OpenLevel(this, CurrentDayRow.NextLevel);
	return true;
}

void ACookieGameJamGameMode::GoToMainMenu()
{
	PrepareForLevelChange();
	UGameplayStatics::OpenLevel(this, MainMenuLevel);
}