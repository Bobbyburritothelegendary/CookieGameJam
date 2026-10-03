// Fill out your copyright notice in the Description page of Project Settings.


#include "AGarryActor.h"
#include "Engine/Engine.h"

AAGarryActor::AAGarryActor()
{
	PromptText = FString("[F] Talk");
}

void AAGarryActor::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
	Super::Interact(Interactor, HeldItem); 

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Garry's function ran."));
	}
}
