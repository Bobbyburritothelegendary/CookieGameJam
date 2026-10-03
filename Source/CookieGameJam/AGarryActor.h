// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AInteractableBase.h"
#include "Engine/DataTable.h"
#include "GarryItemRow.h"
#include "AGarryActor.generated.h"


/**
 * 
 */
UCLASS()
class COOKIEGAMEJAM_API AAGarryActor : public AAInteractableBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orders")
	UDataTable* ItemTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orders")
	FGarryItemRow CurrentOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orders")
	bool bHasOrder = false;

	bool PickRandomItem(FGarryItemRow& OutRow);
	void Say(const FString& Message);
	
	
	
public:
	AAGarryActor();
	
	virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem) override;
	
};
