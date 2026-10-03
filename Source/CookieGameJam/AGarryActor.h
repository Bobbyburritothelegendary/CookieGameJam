// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AInteractableBase.h"
#include "AGarryActor.generated.h"

/**
 * 
 */
UCLASS()
class COOKIEGAMEJAM_API AAGarryActor : public AAInteractableBase
{
	GENERATED_BODY()
	
public:
	AAGarryActor();
	
	virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem) override;
	
};
