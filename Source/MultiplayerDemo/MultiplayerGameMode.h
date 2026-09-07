// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "MultiplayerGameMode.generated.h"

UCLASS()
class MULTIPLAYERDEMO_API AMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	bool bTravelStarted = false;
	bool bTravelStabilizationStarted = false;

	FTimerHandle TravelReadinessTimerHandle;
	FTimerHandle TravelStabilizationTimerHandle;

	void CheckPlayersReadyForTravel();
	void StartTravelAfterStabilization();
};