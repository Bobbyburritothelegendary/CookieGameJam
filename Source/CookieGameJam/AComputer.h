#pragma once

#include "CoreMinimal.h"
#include "AInteractableBase.h"
#include "AComputer.generated.h"

class UCameraComponent;
class UUserWidget;
class USoundBase;
class ACookieGameJamCharacter;

UCLASS()
class COOKIEGAMEJAM_API AAComputer : public AAInteractableBase
{
	GENERATED_BODY()

public:
	AAComputer();

	virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem) override;

	UFUNCTION(BlueprintCallable, Category = "Computer")
	void ExitComputer();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Computer")
	UCameraComponent* ScreenCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Computer")
	TSubclassOf<UUserWidget> ScreenWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Computer")
	float BlendTime = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* StartupSound;
    	
	UPROPERTY()
	UUserWidget* ScreenWidget;

	UPROPERTY()
	ACookieGameJamCharacter* ActivePlayer;
	
	void ShowScreen();
	
	FTimerHandle ShowScreenTimer;
	
	
	
};