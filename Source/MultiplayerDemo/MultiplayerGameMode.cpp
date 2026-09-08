// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	bUseSeamlessTravel = true;
}

void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);


	if (GetNumPlayers() >= 2 && !bTravelStarted)
	{

		if (!GetWorldTimerManager().IsTimerActive(TravelReadinessTimerHandle))
		{
			GetWorldTimerManager().SetTimer(
				TravelReadinessTimerHandle,
				this,
				&AMultiplayerGameMode::CheckPlayersReadyForTravel,
				0.1f,
				true
			);
		}
	}
}

void AMultiplayerGameMode::CheckPlayersReadyForTravel()
{
	if (bTravelStarted)
	{
		GetWorldTimerManager().ClearTimer(TravelReadinessTimerHandle);
		return;
	}

	if (GetNumPlayers() < 2)
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Readiness check failed: World is NULL")
		);

		return;
	}

	bool bAllPlayersReady = true;
	int32 TotalPlayers = 0;

	for (
		FConstPlayerControllerIterator It = World->GetPlayerControllerIterator();
		It;
		++It
		)
	{
		APlayerController* PlayerController = It->Get();

		if (!IsValid(PlayerController))
		{
			continue;
		}

		TotalPlayers++;

		const bool bPlayerReady =
			PlayerController->IsLocalController() ||
			PlayerController->HasClientLoadedCurrentWorld();

		if (!bPlayerReady)
		{
			bAllPlayersReady = false;
		}
	}

	if (!bAllPlayersReady || TotalPlayers < 2)
	{
		return;
	}

	if (bTravelStabilizationStarted)
	{
		return;
	}

	bTravelStabilizationStarted = true;

	GetWorldTimerManager().ClearTimer(
		TravelReadinessTimerHandle
	);

	GetWorldTimerManager().SetTimer(
		TravelStabilizationTimerHandle,
		this,
		&AMultiplayerGameMode::StartTravelAfterStabilization,
		2.0f,
		false
	);
}

void AMultiplayerGameMode::StartTravelAfterStabilization()
{
	if (bTravelStarted)
	{
		return;
	}

	if (GetNumPlayers() < 2)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Travel cancelled during stabilization: player count dropped below 2.")
		);

		bTravelStabilizationStarted = false;
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("ServerTravel failed after stabilization: World is NULL")
		);

		bTravelStabilizationStarted = false;
		return;
	}

	bTravelStarted = true;

	World->ServerTravel(TEXT("/Game/NetTestMap"));
}