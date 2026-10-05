// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AInteractableBase.h"
#include "Engine/DataTable.h"
#include "GarryItemRow.h"
#include "GarryLineRow.h"
#include "AGarryActor.generated.h"

class UTextRenderComponent;
class USoundBase;
class USoundAttenuation;

/**
 * 
 */
UCLASS()
class COOKIEGAMEJAM_API AAGarryActor : public AAInteractableBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orders")
    UDataTable* ItemTable;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orders")
    FGarryItemRow CurrentOrder;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orders")
    bool bHasOrder = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orders")
    float OrderTimeLimit = 60.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speech")
    UTextRenderComponent* SpeechText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speech")
    UDataTable* LineTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    TArray<USoundBase*> TalkSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    USoundBase* CompleteSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    USoundBase* WrongSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    float CharInterval = 0.04f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    float LineHoldTime = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    int32 CharsPerMumble = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    float IdleDelayMin = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    float IdleDelayMax = 40.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    float IdleHearRange = 1500.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    USoundAttenuation* SpeechAttenuation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech")
    float MaxTalkSpeedUp = 3.f;

    bool PickRandomItem(FGarryItemRow& OutRow);
    void Say(const FString& Message);
    void OnOrderTimeout();

    FString PickLine(EGarryLineType Type, FName InItemID, const FString& Fallback) const;
    void Speak(const FString& Text, USoundBase* OneShotSound = nullptr);
    void TypeNextCharacter();
    void ClearSpeech();
    void PlaySpeechSound(USoundBase* Sound, bool bFitToInterval = false);
    void ScheduleIdleLine();
    void SayIdleLine();

    FTimerHandle OrderTimerHandle;
    FTimerHandle TypeTimerHandle;
    FTimerHandle ClearTimerHandle;
    FTimerHandle IdleTimerHandle;

    FString FullText;
    int32 VisibleChars = 0;
    
    bool bUseTalkSounds = true;

public:
    AAGarryActor();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Orders")
    float GetTimeRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Orders")
    FString GetCurrentOrder() const;

    virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem) override;
    
};