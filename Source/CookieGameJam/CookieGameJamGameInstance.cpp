#include "CookieGameJamGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "CookieGameJamGameMode.h"
#include "AudioDevice.h"
#include "Radio.h"

#pragma once

UCookieGameJamGameInstance::UCookieGameJamGameInstance()
{
}

void UCookieGameJamGameInstance::ApplySettings()
{
	// 1. Master Game Volume (Affects whole engine)
	if (MasterSoundMix && MasterSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, MasterSoundClass, GameVolume, 1.0f, 0.0f, true);
		UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
	}
    
	FAudioDeviceHandle AudioDevice = GetWorld()->GetAudioDevice();
	if (AudioDevice)
	{
		AudioDevice->SetTransientPrimaryVolume(GameVolume);
	}

	// 2. Active Music Volume (Background GameMode Music)
	if (ACookieGameJamGameMode* GM = Cast<ACookieGameJamGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->SetMusicVolume(MusicVolume);
	}

	// 3. Update Radio Volume Live
	TArray<AActor*> Radios;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARadio::StaticClass(), Radios);
	for (AActor* RadioActor : Radios)
	{
		if (ARadio* Radio = Cast<ARadio>(RadioActor))
		{
			Radio->PlayCurrentIndex();
		}
	}

	// 4. Player FOV & Sensitivity
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		if (PC->PlayerInput)
		{
			PC->PlayerInput->SetMouseSensitivity(MouseSensitivity);
		}

		ACharacter* Char = Cast<ACharacter>(PC->GetPawn());
		if (Char)
		{
			UCameraComponent* Camera = Char->FindComponentByClass<UCameraComponent>();
			if (Camera)
			{
				Camera->SetFieldOfView(FOV);
			}
		}
	}
}


void UCookieGameJamGameInstance::ResetProgress()
{
	Progress = FPlayerProgress();
}