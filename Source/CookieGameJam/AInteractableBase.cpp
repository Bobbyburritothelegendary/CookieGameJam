// Fill out your copyright notice in the Description page of Project Settings.

#include "AInteractableBase.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

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
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("Item Interacted with."));
    }
}

void AAInteractableBase::Pickup(USceneComponent* AttachToComp)
{
    if (!AttachToComp || bIsPickedUp) return;

    bIsPickedUp = true;

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
}

FString AAInteractableBase::GetPromptText()
{
    return PromptText;
}