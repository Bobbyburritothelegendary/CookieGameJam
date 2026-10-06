// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AInteractableBase.h"
#include "Ladder.generated.h"

class USoundBase;

UCLASS()
class COOKIEGAMEJAM_API ALadder : public AAInteractableBase
{
	GENERATED_BODY()
	
public:
	
	virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* BoostSound;
};
