// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner.h"


// Sets default values
AItemSpawner::AItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	UBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = UBoxComp;
}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (Items.Num() > 0)
	{
		if (SpawnAmount > 0)
		{
			SpawnItems();
		}
	}
	
}


void AItemSpawner::SpawnItems()
{
	const FVector BoxLocation = UBoxComp->GetComponentLocation();
	const FVector HalfSize = UBoxComp->GetScaledBoxExtent();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 I = 0; I < SpawnAmount; ++I)
	{
		TSubclassOf<AAInteractableBase> ItemClass = Items[FMath::RandRange(0, Items.Num() - 1)];
		if (!ItemClass) continue;

		FVector RandomLocation(
			FMath::FRandRange(BoxLocation.X - HalfSize.X, BoxLocation.X + HalfSize.X),
			FMath::FRandRange(BoxLocation.Y - HalfSize.Y, BoxLocation.Y + HalfSize.Y),
			FMath::FRandRange(BoxLocation.Z - HalfSize.Z, BoxLocation.Z + HalfSize.Z)
		);

		GetWorld()->SpawnActor<AAInteractableBase>(ItemClass, RandomLocation, FRotator::ZeroRotator, SpawnParams);
	}
}

// Called every frame
void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

