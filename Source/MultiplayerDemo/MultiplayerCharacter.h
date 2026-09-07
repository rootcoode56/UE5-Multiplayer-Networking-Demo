// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerCharacter.generated.h"

class AReplicatedSwitch;
class AReplicatedPickup;
class UUserWidget;

UCLASS()
class MULTIPLAYERDEMO_API AMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMultiplayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	//Inputs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MouseLookMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> NetRoleLabelClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void OnRep_PlayerState() override;

	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Interact();

	UFUNCTION(Server, Reliable)
	void ServerInteract(AReplicatedSwitch* TargetSwitch);

	UFUNCTION(Server, Reliable)
	void ServerCollectPickup(AReplicatedPickup* TargetPickup);

private:
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> NetRoleLabelWidget;

	void BindPickupCountDelegate();

	UFUNCTION()
	void HandlePickupCountChanged(int32 NewPickupCount);

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
