// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AInteractableBase.generated.h"

UCLASS()
class COOKIEGAMEJAM_API AAInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAInteractableBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString PromptText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsPickedUp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FString GetPromptText();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Pickup(USceneComponent* AttachToComp);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Drop();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanBePickedUp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FName ItemID;
};
