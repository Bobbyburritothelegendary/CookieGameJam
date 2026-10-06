// Fill out your copyright notice in the Description page of Project Settings.

#include "Ladder.h"
#include "CookieGameJamCharacter.h"
#include "Kismet/GameplayStatics.h" 

void ALadder::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
	
	ACookieGameJamCharacter* PlayerChar = Cast<ACookieGameJamCharacter>(Interactor);
	if (PlayerChar)
	{
		PlayerChar->LaunchCharacter(FVector(0.0f, 0.0f, 1200.0f), false, true);
		
		if (BoostSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BoostSound);
		}
	}	
}
