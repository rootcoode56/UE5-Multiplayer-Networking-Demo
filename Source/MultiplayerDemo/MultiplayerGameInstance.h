// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultiplayerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERDEMO_API UMultiplayerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void CreateTestSession();

	UFUNCTION(BlueprintCallable)
	void FindTestSessions();

	UFUNCTION(BlueprintPure)
	UMultiplayerSessionSubsystem* GetMultiplayerSessionSubsystem() const;

	UFUNCTION(BlueprintCallable)
	void JoinTestSessionByRoomCode(const FString& RoomCode);
};
