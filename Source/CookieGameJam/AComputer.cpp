#include "AComputer.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "CookieGameJamCharacter.h"
#include "GameFramework/PlayerController.h"

AAComputer::AAComputer()
{
    PromptText = FString("[F] Use Computer");
    bCanBePickedUp = false;

    ScreenCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ScreenCamera"));
    ScreenCamera->SetupAttachment(RootComponent);
}

void AAComputer::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
    if (ActivePlayer) return;

    ACookieGameJamCharacter* Player = Cast<ACookieGameJamCharacter>(Interactor);
    if (!Player) return;

    APlayerController* PC = Cast<APlayerController>(Player->GetController());
    if (!PC) return;
 
    ActivePlayer = Player;
    Player->ActiveComputer = this;
    Player->GetFirstPersonMesh()->SetVisibility(false);

    PC->SetViewTargetWithBlend(this, BlendTime, VTBlend_Cubic);
    PC->SetIgnoreMoveInput(true);
    PC->SetIgnoreLookInput(true);

    if (BlendTime > 0.f)
    {
        if (StartupSound)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), StartupSound);
        }
        
        GetWorldTimerManager().SetTimer(ShowScreenTimer, this, &AAComputer::ShowScreen, BlendTime, false);
    }
    else
    {
        ShowScreen();
    }
}

void AAComputer::ShowScreen()
{
    if (!ActivePlayer) return;

    APlayerController* PC = Cast<APlayerController>(ActivePlayer->GetController());
    if (!PC) return;

    if (ScreenWidgetClass)
    {
        ScreenWidget = CreateWidget<UUserWidget>(PC, ScreenWidgetClass);
        if (ScreenWidget)
        {
            ScreenWidget->AddToViewport();
        }
    }

    PC->bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
}

void AAComputer::ExitComputer()
{
    if (!ActivePlayer) return;
    
    GetWorldTimerManager().ClearTimer(ShowScreenTimer);

    if (ScreenWidget)
    {
        ScreenWidget->RemoveFromParent();
        ScreenWidget = nullptr;
    }

    APlayerController* PC = Cast<APlayerController>(ActivePlayer->GetController());
    if (PC)
    {
        PC->SetViewTargetWithBlend(ActivePlayer, BlendTime, VTBlend_Cubic);
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    ActivePlayer->GetFirstPersonMesh()->SetVisibility(true);
    ActivePlayer->ActiveComputer = nullptr;
    ActivePlayer = nullptr;
}