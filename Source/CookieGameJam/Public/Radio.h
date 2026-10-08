#pragma once

#include "CoreMinimal.h"
#include "AInteractableBase.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Radio.generated.h"

UCLASS()
class COOKIEGAMEJAM_API ARadio : public AAInteractableBase
{
	GENERATED_BODY()

public:
	ARadio();

	virtual void Interact(AActor* Interactor, AAInteractableBase* HeldItem) override;

	UFUNCTION(BlueprintCallable, Category = "Radio")
	void PlayNextSong();

	UFUNCTION(BlueprintCallable, Category = "Radio")
	void TogglePower();
	
	UFUNCTION(BlueprintCallable, Category = "Radio")
	void PlayCurrentIndex();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Radio|Components")
	UAudioComponent* RadioAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio|Settings")
	bool bIsOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio|Settings")
	float Volume = 0.8f;

private:
	TArray<USoundBase*> CurrentPlaylist;
	int32 CurrentSongIndex = -1;

	void SyncPlaylistFromGameMode();
	void StartRadio();

	UFUNCTION()
	void OnAudioFinished();
};