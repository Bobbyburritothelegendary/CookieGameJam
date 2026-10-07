#include "Radio.h"
#include "CookieGameJamGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ARadio::ARadio()
{
    PrimaryActorTick.bCanEverTick = false;

    RadioAudioComponent =
        CreateDefaultSubobject<UAudioComponent>(
            TEXT("RadioAudioComponent")
        );

    RadioAudioComponent->SetupAttachment(RootComponent);
    RadioAudioComponent->bAutoActivate = false;
    RadioAudioComponent->bIsUISound = false;
    RadioAudioComponent->bAllowSpatialization = true;
}

void ARadio::BeginPlay()
{
    Super::BeginPlay();

    RadioAudioComponent->OnAudioFinished.AddDynamic(
        this,
        &ARadio::OnAudioFinished
    );

    GetWorld()->GetTimerManager().SetTimerForNextTick(
        this,
        &ARadio::StartRadio
    );
}

void ARadio::StartRadio()
{
    const FString LevelName =
        UGameplayStatics::GetCurrentLevelName(this);

    if (LevelName == TEXT("MainMenu"))
    {
        return;
    }

    SyncPlaylistFromGameMode();

    if (CurrentPlaylist.Num() > 0)
    {
        bIsOn = true;
        CurrentSongIndex = -1;
        PlayNextSong();
    }
}

void ARadio::Interact(
    AActor* Interactor,
    AAInteractableBase* HeldItem
)
{
    Super::Interact(
        Interactor,
        HeldItem
    );

    if (CurrentPlaylist.Num() == 0)
    {
        SyncPlaylistFromGameMode();
    }
}

void ARadio::SyncPlaylistFromGameMode()
{
    ACookieGameJamGameMode* GM =
        Cast<ACookieGameJamGameMode>(
            UGameplayStatics::GetGameMode(this)
        );

    if (!GM)
    {
        return;
    }

    CurrentPlaylist =
        GM->GetActivePlaylist();
}

void ARadio::PlayNextSong()
{
    if (CurrentPlaylist.Num() == 0)
    {
        SyncPlaylistFromGameMode();
    }

    if (CurrentPlaylist.Num() == 0)
    {
        return;
    }

    RadioAudioComponent->OnAudioFinished.RemoveDynamic(
        this,
        &ARadio::OnAudioFinished
    );

    if (RadioAudioComponent->IsPlaying())
    {
        RadioAudioComponent->Stop();
    }

    CurrentSongIndex =
        (CurrentSongIndex + 1) %
        CurrentPlaylist.Num();

    bIsOn = true;

    PlayCurrentIndex();

    RadioAudioComponent->OnAudioFinished.AddDynamic(
        this,
        &ARadio::OnAudioFinished
    );
}

void ARadio::TogglePower()
{
    if (bIsOn)
    {
        bIsOn = false;

        RadioAudioComponent->OnAudioFinished.RemoveDynamic(
            this,
            &ARadio::OnAudioFinished
        );

        RadioAudioComponent->Stop();

        RadioAudioComponent->OnAudioFinished.AddDynamic(
            this,
            &ARadio::OnAudioFinished
        );
    }
    else
    {
        bIsOn = true;

        if (CurrentSongIndex == -1)
        {
            PlayNextSong();
        }
        else
        {
            RadioAudioComponent->Play();
        }
    }
}

void ARadio::PlayCurrentIndex()
{
    if (!CurrentPlaylist.IsValidIndex(CurrentSongIndex))
    {
        return;
    }

    USoundBase* SelectedSong =
        CurrentPlaylist[CurrentSongIndex];

    if (!SelectedSong)
    {
        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("RADIO PLAYING: %s"),
        *SelectedSong->GetName()
    );

    RadioAudioComponent->SetSound(SelectedSong);
    RadioAudioComponent->SetVolumeMultiplier(Volume);
    RadioAudioComponent->Play();
}

void ARadio::OnAudioFinished()
{
    if (bIsOn)
    {
        PlayNextSong();
    }
}