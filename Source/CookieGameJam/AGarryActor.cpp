// Fill out your copyright notice in the Description page of Project Settings.


#include "AGarryActor.h"

#include "CookieGameJamCharacter.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"


AAGarryActor::AAGarryActor()
{
	PromptText = FString("[F] Talk");
}

void AAGarryActor::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
	if (!bHasOrder)
	{
		if (PickRandomItem(CurrentOrder))
		{
			bHasOrder = true;
			Say(FString::Printf(TEXT("Bring me a %s"), *CurrentOrder.DisplayName.ToString()));
		}
		else
		{
			Say(TEXT("Garry has no item table assigned."));
		}
		return;
	}

	if (HeldItem && HeldItem->ItemID == CurrentOrder.ItemID)
	{
		
		//Destroy Item and reward Player
		HeldItem->Destroy();
		Say(FString::Printf(TEXT("Nice! Here's $%.2f"), CurrentOrder.Reward));
		
		ACookieGameJamCharacter* MyCharacter = Cast<ACookieGameJamCharacter>(Interactor);
		
		if (MyCharacter)
		{
			MyCharacter->AddCash(CurrentOrder.Reward);
		}
		
		
		bHasOrder = false;
		return;
	}

	Say(FString::Printf(TEXT("I still want a %s"), *CurrentOrder.DisplayName.ToString()));
}

void AAGarryActor::Say(const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, Message);
	}
}
 
bool AAGarryActor::PickRandomItem(FGarryItemRow& OutRow)
{
	if (!ItemTable) return false;

	TArray<FGarryItemRow*> Rows;
	ItemTable->GetAllRows<FGarryItemRow>(TEXT("PickRandomItem"), Rows);
	if (Rows.Num() == 0) return false;

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