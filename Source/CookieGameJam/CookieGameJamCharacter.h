// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AInteractableBase.h"
#include "CookieGameJamCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class AAComputer;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ACookieGameJamCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	//Interact
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction", meta = (AllowPrivateAccess = "true"))
	AAInteractableBase* CurrentInteractable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction", meta = (AllowPrivateAccess = "true"))
	AAInteractableBase* HeldItem;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	//Interact Action
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* InteractAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* TalkAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* ExitAction;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void ExitInteract();
	
	
	UFUNCTION()
	void OnHeldItemDestroyed(AActor* DestroyedActor);
	
public:
	ACookieGameJamCharacter();
	
	virtual void Tick(float DeltaTime) override;

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	//Interact
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void Interact();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void TalkInteract();


protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	//GameplayVariables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float InteractRange = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bDrawDebugLine = false;
	
	
	//Interact
	UFUNCTION(BlueprintImplementableEvent, Category="Gameplay")
	void DisplayInteractText(const FString& Text);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* HoldLocationComponent;
	
	UPROPERTY()
	AAComputer* ActiveComputer;
	
	//Reward
	UFUNCTION(BlueprintImplementableEvent, Category="Gameplay")
	void AddCash(float Amount);
	
	
	//Upgrade
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
	int32 PayRiseLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
	int32 TimeLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
	float PayBonusPerLevel = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
	float TimeBonusPerLevel = 10.f;
};

