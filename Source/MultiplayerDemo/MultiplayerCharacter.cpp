// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ReplicatedSwitch.h"
#include "ReplicatedPickup.h"
#include "MultiplayerPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

// Sets default values
AMultiplayerCharacter::AMultiplayerCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AMultiplayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMultiplayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
			if (MouseLookMappingContext)
			{
				Subsystem->AddMappingContext(MouseLookMappingContext, 1);
			}
		}
	}

	if (!IsLocallyControlled())
	{
		return;
	}

	APlayerController* LocalPlayerController =
		Cast<APlayerController>(Controller);

	if (!LocalPlayerController || !NetRoleLabelClass)
	{
		return;
	}

	const ENetMode NetMode = GetNetMode();

	if (!IsValid(NetRoleLabelWidget))
	{
		NetRoleLabelWidget =
			CreateWidget<UUserWidget>(
				LocalPlayerController,
				NetRoleLabelClass
			);

		if (NetRoleLabelWidget)
		{
			if (UTextBlock* RoleText =
				Cast<UTextBlock>(
					NetRoleLabelWidget->GetWidgetFromName(
						TEXT("RoleText")
					)
				))
			{
				const FText RoleLabel =
					(NetMode == NM_ListenServer)
					? FText::FromString(TEXT("HOST POV"))
					: FText::FromString(TEXT("CLIENT POV"));

				RoleText->SetText(RoleLabel);
			}

			NetRoleLabelWidget->AddToPlayerScreen(100);
		}
	}

	BindPickupCountDelegate();
}

void AMultiplayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocallyControlled())
	{
		BindPickupCountDelegate();
	}
}

void AMultiplayerCharacter::BindPickupCountDelegate()
{
	if (!IsLocallyControlled() || !IsValid(NetRoleLabelWidget))
	{
		return;
	}

	AMultiplayerPlayerState* MultiplayerPlayerState =
		Cast<AMultiplayerPlayerState>(GetPlayerState());

	if (!MultiplayerPlayerState)
	{
		return;
	}

	MultiplayerPlayerState->OnPickupCountChanged.RemoveDynamic(
		this,
		&AMultiplayerCharacter::HandlePickupCountChanged
	);

	MultiplayerPlayerState->OnPickupCountChanged.AddDynamic(
		this,
		&AMultiplayerCharacter::HandlePickupCountChanged
	);

	HandlePickupCountChanged(
		MultiplayerPlayerState->GetPickupCount()
	);
}

void AMultiplayerCharacter::HandlePickupCountChanged(
	int32 NewPickupCount
)
{
	if (!IsValid(NetRoleLabelWidget))
	{
		return;
	}

	UTextBlock* PickupCountText =
		Cast<UTextBlock>(
			NetRoleLabelWidget->GetWidgetFromName(
				TEXT("PickupCountText")
			)
		);

	if (!PickupCountText)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PickupCountText not found in NetRoleLabelWidget")
		);

		return;
	}

	PickupCountText->SetText(
		FText::FromString(
			FString::Printf(
				TEXT("PICKUPS: %d"),
				NewPickupCount
			)
		)
	);
}

// Called to bind functionality to input
void AMultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(
				MoveAction,
				ETriggerEvent::Triggered,
				this,
				&AMultiplayerCharacter::Move
			);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(
				LookAction,
				ETriggerEvent::Triggered,
				this,
				&AMultiplayerCharacter::Look
			);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(
				JumpAction,
				ETriggerEvent::Started,
				this,
				&ACharacter::Jump
			);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(
				InteractAction,
				ETriggerEvent::Started,
				this,
				&AMultiplayerCharacter::Interact
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent cast FAILED"));
	}
}
void AMultiplayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (!Controller)
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();

	const FRotator YawRotation(
		0.0f,
		ControlRotation.Yaw,
		0.0f
	);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMultiplayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AMultiplayerCharacter::Interact()
{
	if (!FollowCamera)
	{
		return;
	}

	const FVector Start = FollowCamera->GetComponentLocation();

	const FVector End =
		Start + FollowCamera->GetForwardVector() * 600.0f;

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	if (!IsValid(HitActor))
	{
		return;
	}

	if (AReplicatedSwitch* TargetSwitch =
		Cast<AReplicatedSwitch>(HitActor))
	{
		ServerInteract(TargetSwitch);

		return;
	}

	if (AReplicatedPickup* TargetPickup =
		Cast<AReplicatedPickup>(HitActor))
	{
		ServerCollectPickup(TargetPickup);

		return;
	}
}

void AMultiplayerCharacter::ServerInteract_Implementation(
	AReplicatedSwitch* TargetSwitch
)
{
	if (!IsValid(TargetSwitch))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ServerInteract rejected: TargetSwitch is invalid")
		);

		return;
	}

	constexpr float MaxInteractionDistance = 300.0f;

	const float DistanceSquared = FVector::DistSquared(
		GetActorLocation(),
		TargetSwitch->GetActorLocation()
	);

	if (DistanceSquared > FMath::Square(MaxInteractionDistance))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ServerInteract rejected: switch is too far away")
		);

		return;
	}

	TargetSwitch->ToggleSwitch();
}

void AMultiplayerCharacter::ServerCollectPickup_Implementation(
	AReplicatedPickup* TargetPickup
)
{
	if (!IsValid(TargetPickup))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ServerCollectPickup rejected: TargetPickup is invalid")
		);

		return;
	}

	constexpr float MaxPickupDistance = 300.0f;

	const float DistanceSquared = FVector::DistSquared(
		GetActorLocation(),
		TargetPickup->GetActorLocation()
	);

	if (DistanceSquared > FMath::Square(MaxPickupDistance))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ServerCollectPickup rejected: pickup is too far away")
		);

		return;
	}

	AMultiplayerPlayerState* MultiplayerPlayerState =
		Cast<AMultiplayerPlayerState>(GetPlayerState());

	if (!MultiplayerPlayerState)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("ServerCollectPickup rejected: MultiplayerPlayerState is invalid")
		);

		return;
	}

	MultiplayerPlayerState->AddPickup();

	TargetPickup->CollectPickup();
}

