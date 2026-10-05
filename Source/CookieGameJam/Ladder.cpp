// Fill out your copyright notice in the Description page of Project Settings.


#include "Ladder.h"
#include "CookieGameJamCharacter.h"


void ALadder::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Ladder interact ran"));
	}
	
	
	ACookieGameJamCharacter* PlayerChar = Cast<ACookieGameJamCharacter>(Interactor);
	if (PlayerChar)
	{
		PlayerChar->LaunchCharacter(FVector(0.0f, 0.0f, 1200.0f), false, true);
	}	
}
