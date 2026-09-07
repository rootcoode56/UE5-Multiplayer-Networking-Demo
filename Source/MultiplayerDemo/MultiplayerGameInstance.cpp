// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"
#include "MultiplayerSessionSubsystem.h"

void UMultiplayerGameInstance::CreateTestSession()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateTestSession: World is NULL"));
		return;
	}

	if (!World->GetAuthGameMode())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CreateTestSession ignored: This instance is not the Host.")
		);

		return;
	}

	UMultiplayerSessionSubsystem* SessionSubsystem =
		GetSubsystem<UMultiplayerSessionSubsystem>();

	if (SessionSubsystem)
	{
		SessionSubsystem->CreateSession();
	}
}

void UMultiplayerGameInstance::FindTestSessions()
{
	UMultiplayerSessionSubsystem* SessionSubsystem =
		GetSubsystem<UMultiplayerSessionSubsystem>();

	if (SessionSubsystem)
	{
		SessionSubsystem->FindSessions();
	}
}

void UMultiplayerGameInstance::JoinTestSessionByRoomCode(
	const FString& RoomCode)
{
	UMultiplayerSessionSubsystem* SessionSubsystem =
		GetSubsystem<UMultiplayerSessionSubsystem>();

	if (SessionSubsystem)
	{
		SessionSubsystem->JoinSessionByRoomCode(RoomCode);
	}
}

UMultiplayerSessionSubsystem*
UMultiplayerGameInstance::GetMultiplayerSessionSubsystem() const
{
	return GetSubsystem<UMultiplayerSessionSubsystem>();
}