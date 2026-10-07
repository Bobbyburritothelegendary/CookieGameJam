
// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookieGameJamCharacter.h"

#include "AGarryActor.h"
#include "AComputer.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CookieGameJam.h"
#include "DrawDebugHelpers.h"  
#include "Engine/Engine.h"

ACookieGameJamCharacter::ACookieGameJamCharacter()
{
	PrimaryActorTick.bCanEverTick = true; 
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	//Interact
	HoldLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldLocationComponent"));
	HoldLocationComponent->SetRelativeLocation(FVector(70.0f, 35.0f, -25.0f));
	HoldLocationComponent->SetupAttachment(FirstPersonCameraComponent);
}

void ACookieGameJamCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!FirstPersonCameraComponent || !GetWorld()) return;
	
	if (ActiveComputer != nullptr)
	{
		DisplayInteractText(FString(""));
		CurrentInteractable = nullptr;
		return;
	}
	
	
	//Find Interactables using raycast
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	
	FVector StartLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndLocation = StartLocation + (ForwardVector * InteractRange);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	if (HeldItem)
	{
		CollisionParams.AddIgnoredActor(HeldItem);
	}
	FHitResult HitResult;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		CollisionParams
	);
	
	if (bDrawDebugLine)
	{
		DrawDebugLine(GetWorld(),
		StartLocation,
		EndLocation,
		bHit ? FColor::Green : FColor::Red,
		false,
		1.0f,
		0,
		1.5f
		);
	}
	
	
	
	if (GEngine)
	{
		if (bHit && HitResult.GetActor())
		{
			AAInteractableBase* HitInteractable = Cast<AAInteractableBase>(HitResult.GetActor());
			
			if (HitInteractable)
			{
				CurrentInteractable = HitInteractable;
				// GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Red, FString::Printf(TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName()));
				
				if (HeldItem)
				{
					if (HitInteractable->bCanBePickedUp == false)
					{
						FString PromptText = CurrentInteractable->GetPromptText();
						// GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Red, PromptText);
						DisplayInteractText(PromptText);
					}
					else
					{
						DisplayInteractText(FString("[E] Drop"));
					}
				}
				else
				{
					FString PromptText = CurrentInteractable->GetPromptText();
					// GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Red, PromptText);
				
					DisplayInteractText(PromptText);
				}
			}
			else
			{
				CurrentInteractable = nullptr;
				// GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("No Interactable Actors Hit."));
				
				if (HeldItem)
				{
					DisplayInteractText(FString("[E] Drop"));
				}
				else
				{
					DisplayInteractText(FString(""));
				}
				
			}
		}
		else
		{
			CurrentInteractable = nullptr;
			// GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("Nothing hit."));
			
			DisplayInteractText(FString(""));
		}
	}
	
	//Footstep Sounds
	if (GetCharacterMovement() && GetCharacterMovement()->IsMovingOnGround())
	{
		// Calculate actual 2D velocity (ignoring Z movement like jumping/falling)
		FVector Velocity = GetVelocity();
		Velocity.Z = 0.0f;
		float Speed = Velocity.Size();

		if (Speed > 10.0f) // Character is moving
		{
			DistanceTraveled += Speed * DeltaTime;

			if (DistanceTraveled >= DistancePerFootstep)
            {
                DistanceTraveled = 0.0f;
                PlayFootstep();
            }
		}
	}
	else
	{
		DistanceTraveled = 0.0f; // Reset when in air or stopped
	}
}

//Gameplay 
void ACookieGameJamCharacter::Interact()
{
	if (ActiveComputer)
	{
		DisplayInteractText(FString(""));
		return;
	}
	
	if (!HeldItem)
	{
		if (CurrentInteractable)
		{
			if (CurrentInteractable->bCanBePickedUp == true)
			{
				HeldItem = CurrentInteractable;
				HeldItem->Pickup(HoldLocationComponent);
				
				HeldItem->OnDestroyed.AddDynamic(this, &ACookieGameJamCharacter::OnHeldItemDestroyed);
				
				DisplayInteractText(FString("[E] Drop"));
			}
			
		}
	}
	else
	{
		HeldItem->Drop();
		HeldItem->OnDestroyed.RemoveDynamic(this, &ACookieGameJamCharacter::OnHeldItemDestroyed);
		HeldItem = nullptr;
		
		DisplayInteractText(FString(""));
	}
}

void ACookieGameJamCharacter::TalkInteract()
{
	if (CurrentInteractable)
	{
		CurrentInteractable->Interact(this, HeldItem);
	}
}

void ACookieGameJamCharacter::ExitInteract()
{
	if (ActiveComputer)
	{
		ActiveComputer->ExitComputer();
	}
}

void ACookieGameJamCharacter::OnHeldItemDestroyed(AActor* DestroyedActor)
{
	if (HeldItem == DestroyedActor)
	{
		HeldItem = nullptr;
		
		DisplayInteractText(FString(""));
        
		// if (GEngine)
		// {
		// 	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Held item was destroyed!"));
		// }
	}
}

void ACookieGameJamCharacter::PlayFootstep()
{
	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, HalfHeight + 50.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;
	Params.bTraceComplex = true;

	EPhysicalSurface Surface = SurfaceType_Default;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		Surface = UGameplayStatics::GetSurfaceType(Hit);
	}

	const FFootstepSet* Set = FootstepSounds.Find(Surface);
	if (!Set || Set->Sounds.Num() == 0)
	{
		Set = &DefaultFootsteps;
	}
	if (Set->Sounds.Num() == 0) return;

	USoundBase* Sound = Set->Sounds[FootstepIndex % Set->Sounds.Num()];
	FootstepIndex++;

	if (Sound)
	{
		const FVector FeetLocation = Start - FVector(0.f, 0.f, HalfHeight);
		UGameplayStatics::PlaySoundAtLocation(this, Sound, FeetLocation, FMath::FRandRange(0.8f, 1.f), FMath::FRandRange(0.95f, 1.05f));
	}
}

#include "Radio.h"

void ACookieGameJamCharacter::OnPrimaryAction()
{
	if (ARadio* Radio = Cast<ARadio>(HeldItem))
	{
		Radio->PlayNextSong();
	}
}

void ACookieGameJamCharacter::OnSecondaryAction()
{
	if (ARadio* Radio = Cast<ARadio>(HeldItem))
	{
		Radio->TogglePower();
	}
}



//PlayerInput and movement
void ACookieGameJamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACookieGameJamCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACookieGameJamCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACookieGameJamCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACookieGameJamCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACookieGameJamCharacter::LookInput);
		
		//Interactions
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACookieGameJamCharacter::Interact);
		EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Started, this, &ACookieGameJamCharacter::TalkInteract);
		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Started, this, &ACookieGameJamCharacter::ExitInteract);
		
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ACookieGameJamCharacter::OnPrimaryAction);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ACookieGameJamCharacter::OnSecondaryAction);
		
	}
	else
	{
		UE_LOG(LogCookieGameJam, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ACookieGameJamCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ACookieGameJamCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ACookieGameJamCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACookieGameJamCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ACookieGameJamCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ACookieGameJamCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}