#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerProgress.h"
#include "CookieGameJamGameInstance.generated.h"

UCLASS()
class COOKIEGAMEJAM_API UCookieGameJamGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCookieGameJamGameInstance();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float GameVolume = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MusicVolume = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float FOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MouseSensitivity = 1.0f;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	class USoundMix* MasterSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	class USoundClass* MasterSoundClass;
	
	
	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	FPlayerProgress Progress;

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void ResetProgress();
};