// Fill out your copyright notice in the Description page of Project Settings.

#include "AInteractableBase.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AAInteractableBase::AAInteractableBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
    RootComponent = StaticMeshComp;
    PromptText = FString("[E] Pick Up"); //Default Interact Prompt
    bIsPickedUp = false;
    bCanBePickedUp = true;
    
    static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundFinder(TEXT("/Engine/VREditor/Sounds/UI/Object_PickUp.Object_PickUp"));
    if (PickupSoundFinder.Succeeded())
    {
        PickupSound = PickupSoundFinder.Object;
    }
}

// Called when the game starts or when spawned
void AAInteractableBase::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AAInteractableBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAInteractableBase::Interact(AActor* Interactor, AAInteractableBase* HeldItem)
{
    // if (GEngine)
    // {
    //     GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("Item Interacted with."));
    // }
}

void AAInteractableBase::Pickup(USceneComponent* AttachToComp)
{
    if (!AttachToComp || bIsPickedUp) return;

    bIsPickedUp = true;
    
    if (PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
    }

    TArray<UPrimitiveComponent*> PrimitiveComps;
    GetComponents<UPrimitiveComponent>(PrimitiveComps);

    for (UPrimitiveComponent* PrimComp : PrimitiveComps)
    {
        PrimComp->SetSimulatePhysics(false);
        PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    FAttachmentTransformRules AttachmentRules(
        EAttachmentRule::SnapToTarget, // Snaps Location
        EAttachmentRule::SnapToTarget, // Snaps Rotation
        EAttachmentRule::KeepWorld,    // Keeps existing scale
        false
    );

    AttachToComponent(AttachToComp, AttachmentRules);
    
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerChar)
    {
        UFunction* UpdateFunc = PlayerChar->FindFunction(FName("UpdateHUDControls"));
        if (UpdateFunc)
        {
            AAInteractableBase* ItemToPass = this;
            PlayerChar->ProcessEvent(UpdateFunc, &ItemToPass);
        }
    }
}

void AAInteractableBase::Drop()
{
    if (!bIsPickedUp) return;

    DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

    TArray<UPrimitiveComponent*> PrimitiveComps;
    GetComponents<UPrimitiveComponent>(PrimitiveComps);

    for (UPrimitiveComponent* PrimComp : PrimitiveComps)
    {
        PrimComp->SetCollisionProfileName(TEXT("Pickup"));
        PrimComp->SetSimulatePhysics(true);
    }

    bIsPickedUp = false;
    
    if (DropSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
    }
    
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerChar)
    {
        UFunction* UpdateFunc = PlayerChar->FindFunction(FName("UpdateHUDControls"));
        if (UpdateFunc)
        {
            AAInteractableBase* ItemToPass = nullptr; // Clear held item
            PlayerChar->ProcessEvent(UpdateFunc, &ItemToPass);
        }
    }
}

FString AAInteractableBase::GetPromptText()
{
    return PromptText;
}

void AAInteractableBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (ItemMesh)
    {
        StaticMeshComp->SetStaticMesh(ItemMesh);
    }
}