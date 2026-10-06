// Fill out your copyright notice in the Description page of Project Settings.


#include "AGarryActor.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/TextRenderComponent.h"
#include "CookieGameJamCharacter.h"
#include "CookieGameJamGameMode.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "TimerManager.h"

AAGarryActor::AAGarryActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PromptText = FString("[F] Talk");

    SpeechText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SpeechText"));
    SpeechText->SetupAttachment(RootComponent);
    SpeechText->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
    SpeechText->SetHorizontalAlignment(EHTA_Center);
    SpeechText->SetVerticalAlignment(EVRTA_TextBottom);
    SpeechText->SetWorldSize(24.f);
    SpeechText->SetText(FText::GetEmpty());
}

void AAGarryActor::BeginPlay()
{
    Super::BeginPlay();
    ScheduleIdleLine();
}

void AAGarryActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FullText.IsEmpty()) return;

    APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(this, 0);
    if (!Camera) return;

    const FRotator Look = UKismetMathLibrary::FindLookAtRotation(SpeechText->GetComponentLocation(), Camera->GetCameraLocation());
    SpeechText->SetWorldRotation(FRotator(0.f, Look.Yaw, 0.f));
}

void AAGarryActor::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
	ACookieGameJamCharacter* MyCharacter = Cast<ACookieGameJamCharacter>(Interactor);

	if (!bHasOrder)
	{
		if (PickRandomItem(CurrentOrder))
		{
			if (OrderTimeLimit > 0.f)
			{
				float TimeLimit = OrderTimeLimit;
				if (MyCharacter)
				{
					TimeLimit += (MyCharacter->TimeLevel - 1) * MyCharacter->TimeBonusPerLevel;
				}
				GetWorldTimerManager().SetTimer(OrderTimerHandle, this, &AAGarryActor::OnOrderTimeout, TimeLimit, false);
			}

			bHasOrder = true;

			Speak(PickLine(EGarryLineType::Request, CurrentOrder.ItemID, TEXT("Bring me a {Item}")));
		}
		else
		{
			Say(TEXT("Garry has no item table assigned."));
		}
		return;
	}

	if (HeldItem && HeldItem->ItemID == CurrentOrder.ItemID)
	{
		if (MyCharacter)
		{
			CurrentOrder.Reward *= 1.f + (MyCharacter->PayRiseLevel - 1) * MyCharacter->PayBonusPerLevel;
			MyCharacter->AddCash(CurrentOrder.Reward);
		}

		//Destroy Item and reward Player
		HeldItem->Destroy();
		GetWorldTimerManager().ClearTimer(OrderTimerHandle);
		Speak(PickLine(EGarryLineType::Complete, CurrentOrder.ItemID, TEXT("Nice! Here's ${Reward}")), CompleteSound);
		
		if (CompleteSound2)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), CompleteSound2);
		}
		
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
		if (ACookieGameJamGameMode* CustomGameMode = Cast<ACookieGameJamGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			CustomGameMode->OnJobCompleted();
		}

		bHasOrder = false;
		return;
	}

	Speak(PickLine(EGarryLineType::Reminder, CurrentOrder.ItemID, TEXT("I still want a {Item}")), WrongSound);
}

void AAGarryActor::Say(const FString& Message)
{
    if (GEngine)
    {
       GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, Message);
    }
}

void AAGarryActor::OnOrderTimeout()
{
    bHasOrder = false;
    Speak(PickLine(EGarryLineType::Timeout, CurrentOrder.ItemID, TEXT("Too slow! Forget it.")));
}

bool AAGarryActor::SkipOrder(AActor* Interactor)
{
	if (!bHasOrder) return false;

	GetWorldTimerManager().ClearTimer(OrderTimerHandle);
	bHasOrder = false;
	//
	return true;
}

FString AAGarryActor::PickLine(EGarryLineType Type, FName InItemID, const FString& Fallback) const
{
    FString Result = Fallback;

    if (LineTable)
    {
       TArray<FGarryLineRow*> Rows;
       LineTable->GetAllRows<FGarryLineRow>(TEXT("PickLine"), Rows);

       TArray<const FGarryLineRow*> Specific;
       TArray<const FGarryLineRow*> Generic;

       for (const FGarryLineRow* Row : Rows)
       {
          if (Row->Type != Type) continue;

       	if (!InItemID.IsNone() && Row->ItemID == InItemID)
          {
             Specific.Add(Row);
          }
          else if (Row->ItemID.IsNone())
          {
             Generic.Add(Row);
          }
       }

       const TArray<const FGarryLineRow*>& Pool = Specific.Num() > 0 ? Specific : Generic;
       if (Pool.Num() > 0)
       {
          Result = Pool[FMath::RandRange(0, Pool.Num() - 1)]->Line;
       }
    }

    Result = Result.Replace(TEXT("{Item}"), *CurrentOrder.DisplayName.ToString());
    Result = Result.Replace(TEXT("{Reward}"), *FString::Printf(TEXT("%.0f"), CurrentOrder.Reward));
    return Result;
}

void AAGarryActor::Speak(const FString& Text, USoundBase* OneShotSound)
{
	FTimerManager& TM = GetWorldTimerManager();
	TM.ClearTimer(TypeTimerHandle);
	TM.ClearTimer(ClearTimerHandle);

	FullText = Text;
	VisibleChars = 0;
	SpeechText->SetText(FText::GetEmpty());

	if (FullText.IsEmpty()) return;

	bUseTalkSounds = (OneShotSound == nullptr);
	if (OneShotSound)
	{
		PlaySpeechSound(OneShotSound);
	}

	TM.SetTimer(TypeTimerHandle, this, &AAGarryActor::TypeNextCharacter, FMath::Max(CharInterval, 0.01f), true);
}

void AAGarryActor::TypeNextCharacter()
{
	VisibleChars++;
	SpeechText->SetText(FText::FromString(FullText.Left(VisibleChars)));

	const bool bIsLetter = !FChar::IsWhitespace(FullText[VisibleChars - 1]);
	if (bUseTalkSounds && bIsLetter && TalkSounds.Num() > 0 && VisibleChars % FMath::Max(CharsPerMumble, 1) == 0)
	{
		PlaySpeechSound(TalkSounds[FMath::RandRange(0, TalkSounds.Num() - 1)]);
	}

	if (VisibleChars >= FullText.Len())
	{
		GetWorldTimerManager().ClearTimer(TypeTimerHandle);
		GetWorldTimerManager().SetTimer(ClearTimerHandle, this, &AAGarryActor::ClearSpeech, FMath::Max(LineHoldTime, 0.1f), false);
	}
}

void AAGarryActor::PlaySpeechSound(USoundBase* Sound, bool bFitToInterval)
{
	if (!Sound) return;

	float Pitch = FMath::FRandRange(0.9f, 1.1f);

	if (bFitToInterval)
	{
		const float Interval = FMath::Max(CharInterval, 0.01f) * FMath::Max(CharsPerMumble, 1);
		const float Duration = Sound->GetDuration();
		if (Duration > Interval && Duration < 100.f)
		{
			Pitch *= FMath::Min(Duration / Interval, MaxTalkSpeedUp);
		}
	}

	Pitch = FMath::Clamp(Pitch, 0.5f, 4.f);

	UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation(), 1.f, Pitch, 0.f, SpeechAttenuation);
}

void AAGarryActor::ClearSpeech()
{
    FullText.Empty();
    VisibleChars = 0;
    SpeechText->SetText(FText::GetEmpty());
}

void AAGarryActor::ScheduleIdleLine()
{
    if (IdleDelayMax <= 0.f) return;

    const float Delay = FMath::Max(FMath::FRandRange(IdleDelayMin, IdleDelayMax), 1.f);
    GetWorldTimerManager().SetTimer(IdleTimerHandle, this, &AAGarryActor::SayIdleLine, Delay, false);
}

void AAGarryActor::SayIdleLine()
{
    FTimerManager& TM = GetWorldTimerManager();
    const bool bBusy = TM.IsTimerActive(TypeTimerHandle) || TM.IsTimerActive(ClearTimerHandle);

    APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
    const bool bNear = Player && FVector::Dist(Player->GetActorLocation(), GetActorLocation()) <= IdleHearRange;

    if (!bBusy && bNear)
    {
       const FString Line = PickLine(EGarryLineType::Idle, NAME_None, FString());
       if (!Line.IsEmpty())
       {
          Speak(Line);
       }
    }

    ScheduleIdleLine();
}

//HELPER FUNCTIONS

float AAGarryActor::GetTimeRemaining() const
{
    return GetWorldTimerManager().GetTimerRemaining(OrderTimerHandle);
}

FString AAGarryActor::GetCurrentOrder() const
{
    return CurrentOrder.DisplayName.ToString();
}

bool AAGarryActor::PickRandomItem(FGarryItemRow& OutRow)
{
	if (!ItemTable) return false;

	TArray<FGarryItemRow*> AllRows;
	ItemTable->GetAllRows<FGarryItemRow>(TEXT("PickRandomItem"), AllRows);
	if (AllRows.Num() == 0) return false;

	const FName PreviousID = OutRow.ItemID;

	TArray<FGarryItemRow*> Rows;
	for (FGarryItemRow* Row : AllRows)
	{
		if (AllRows.Num() == 1 || Row->ItemID != PreviousID)
		{
			Rows.Add(Row);
		}
	}
	if (Rows.Num() == 0)
	{
		Rows = AllRows;
	}

	float Total = 0.f;
	for (const FGarryItemRow* Row : Rows)
	{
		Total += Row->Weight;
	}
	if (Total <= 0.f) return false;

	float Roll = FMath::FRandRange(0.f, Total);
	for (const FGarryItemRow* Row : Rows)
	{
		Roll -= Row->Weight;
		if (Roll < 0.f)
		{
			OutRow = *Row;
			return true;
		}
	}

	OutRow = *Rows.Last();
	return true;
}