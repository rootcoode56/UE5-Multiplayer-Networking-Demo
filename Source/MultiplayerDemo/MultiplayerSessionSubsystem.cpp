// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

void UMultiplayerSessionSubsystem::Initialize(
	FSubsystemCollectionBase& Collection
)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Online Subsystem: NULL")
		);

		return;
	}

	SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Session Interface: INVALID")
		);
	}
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(
	FName SessionName,
	bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteDelegateHandle
		);

		CreateSessionCompleteDelegateHandle.Reset();
	}

	if (!bWasSuccessful)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CreateSession failed")
		);
	}

	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionSubsystem::OnFindSessionsComplete(
	bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteDelegateHandle
		);

		FindSessionsCompleteDelegateHandle.Reset();
	}

	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Session search failed or search object is invalid")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Unable to search for rooms")
		);

		return;
	}

	for (int32 Index = 0; Index < SessionSearch->SearchResults.Num(); ++Index)
	{
		const FOnlineSessionSearchResult& Result =
			SessionSearch->SearchResults[Index];

		FString RoomCode;

		if (Result.Session.SessionSettings.Get(
			TEXT("ROOM_CODE"),
			RoomCode))
		{
			if (RoomCode.Equals(
				DesiredRoomCode,
				ESearchCase::IgnoreCase))
			{
				JoinSessionCompleteDelegate =
					FOnJoinSessionCompleteDelegate::CreateUObject(
						this,
						&UMultiplayerSessionSubsystem::OnJoinSessionComplete
					);

				JoinSessionCompleteDelegateHandle =
					SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
						JoinSessionCompleteDelegate
					);

				const FName SessionName(TEXT("MultiplayerDemoSession"));

				const bool bJoinStarted =
					SessionInterface->JoinSession(
						0,
						SessionName,
						Result
					);

				if (!bJoinStarted)
				{
					UE_LOG(
						LogTemp,
						Error,
						TEXT("JoinSession request failed to start")
					);

					if (JoinSessionCompleteDelegateHandle.IsValid())
					{
						SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
							JoinSessionCompleteDelegateHandle
						);

						JoinSessionCompleteDelegateHandle.Reset();
					}

					OnRoomJoinFailed.Broadcast(
						TEXT("Unable to join room")
					);

					return;
				}

				return;
			}
		}
	}

	OnRoomJoinFailed.Broadcast(
		TEXT("Room not found")
	);
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(
	FName SessionName,
	EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
			JoinSessionCompleteDelegateHandle
		);

		JoinSessionCompleteDelegateHandle.Reset();
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("JoinSession failed")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Unable to join room")
		);

		return;
	}

	FString Address;

	if (!SessionInterface->GetResolvedConnectString(
		SessionName,
		Address))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to resolve session connect string")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Unable to resolve server address")
		);

		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("JoinSession failed: World is invalid")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Unable to access game world")
		);

		return;
	}

	APlayerController* PlayerController =
		World->GetFirstPlayerController();

	if (!PlayerController)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("JoinSession failed: PlayerController is invalid")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Unable to access local player")
		);

		return;
	}

	PlayerController->ClientTravel(
		Address,
		TRAVEL_Absolute
	);
}

FString UMultiplayerSessionSubsystem::GenerateRoomCode()
{
	const FString Characters = TEXT("ABCDEFGHJKLMNPQRSTUVWXYZ23456789");

	FString RoomCode;

	for (int32 Index = 0; Index < 6; ++Index)
	{
		const int32 RandomIndex = FMath::RandRange(0, Characters.Len() - 1);
		RoomCode.AppendChar(Characters[RandomIndex]);
	}

	return RoomCode;
}

void UMultiplayerSessionSubsystem::CreateSession()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CreateSession failed: Session Interface invalid")
		);

		return;
	}

	const FName SessionName(TEXT("MultiplayerDemoSession"));

	if (SessionInterface->GetNamedSession(SessionName))
	{

		bFindSessionsOnDestroy = false;
		bCreateSessionOnDestroy = true;

		DestroySession();

		return;
	}

	CreateSessionCompleteDelegate =
		FOnCreateSessionCompleteDelegate::CreateUObject(
			this,
			&UMultiplayerSessionSubsystem::OnCreateSessionComplete
		);

	CreateSessionCompleteDelegateHandle =
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteDelegate
		);

	CurrentRoomCode = GenerateRoomCode();

	FOnlineSessionSettings SessionSettings;

	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;

	SessionSettings.Set(
		TEXT("ROOM_CODE"),
		CurrentRoomCode,
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
	);

	const bool bCreateStarted =
		SessionInterface->CreateSession(
			0,
			SessionName,
			SessionSettings
		);

	if (!bCreateStarted)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CreateSession request failed to start")
		);

		if (CreateSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
				CreateSessionCompleteDelegateHandle
			);

			CreateSessionCompleteDelegateHandle.Reset();
		}

		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionSubsystem::FindSessions()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("FindSessions failed: Session Interface invalid")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Online session service unavailable")
		);

		return;
	}

	FindSessionsCompleteDelegate =
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&UMultiplayerSessionSubsystem::OnFindSessionsComplete
		);

	FindSessionsCompleteDelegateHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteDelegate
		);

	SessionSearch = MakeShared<FOnlineSessionSearch>();

	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->PingBucketSize = 50;

	const bool bFindStarted = SessionInterface->FindSessions(
		0,
		SessionSearch.ToSharedRef()
	);


	if (!bFindStarted)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("FindSessions request failed to start")
		);

		if (SessionInterface.IsValid() &&
			FindSessionsCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
				FindSessionsCompleteDelegateHandle
			);

			FindSessionsCompleteDelegateHandle.Reset();
		}

		OnRoomJoinFailed.Broadcast(
			TEXT("Unable to search for rooms")
		);
	}
}

void UMultiplayerSessionSubsystem::JoinSessionByRoomCode(
	const FString& RoomCode)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("JoinSessionByRoomCode failed: Session Interface invalid")
		);

		OnRoomJoinFailed.Broadcast(
			TEXT("Online session service unavailable")
		);

		return;
	}

	DesiredRoomCode = RoomCode;

	const FName SessionName(TEXT("MultiplayerDemoSession"));

	if (SessionInterface->GetNamedSession(SessionName))
	{
		bCreateSessionOnDestroy = false;
		bFindSessionsOnDestroy = true;

		DestroySession();

		return;
	}

	FindSessions();
}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(
	FName SessionName,
	bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(
			DestroySessionCompleteDelegateHandle
		);

		DestroySessionCompleteDelegateHandle.Reset();
	}

	if (!bWasSuccessful)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("DestroySession failed. Pending session operation cancelled.")
		);

		const bool bWasPendingRoomSearch =
			bFindSessionsOnDestroy;

		bCreateSessionOnDestroy = false;
		bFindSessionsOnDestroy = false;

		if (bWasPendingRoomSearch)
		{
			OnRoomJoinFailed.Broadcast(
				TEXT("Unable to prepare room search")
			);
		}

		return;
	}

	if (bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;

		CreateSession();

		return;
	}

	if (bFindSessionsOnDestroy)
	{
		bFindSessionsOnDestroy = false;

		FindSessions();
	}
}

void UMultiplayerSessionSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("DestroySession failed: Session Interface invalid")
		);

		return;
	}

	DestroySessionCompleteDelegate =
		FOnDestroySessionCompleteDelegate::CreateUObject(
			this,
			&UMultiplayerSessionSubsystem::OnDestroySessionComplete
		);

	DestroySessionCompleteDelegateHandle =
		SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			DestroySessionCompleteDelegate
		);

	const bool bDestroyStarted =
		SessionInterface->DestroySession(
			FName(TEXT("MultiplayerDemoSession"))
		);

	if (!bDestroyStarted)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("DestroySession request failed to start")
		);

		if (DestroySessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(
				DestroySessionCompleteDelegateHandle
			);

			DestroySessionCompleteDelegateHandle.Reset();
		}

		const bool bWasPendingRoomSearch =
			bFindSessionsOnDestroy;

		bCreateSessionOnDestroy = false;
		bFindSessionsOnDestroy = false;

		if (bWasPendingRoomSearch)
		{
			OnRoomJoinFailed.Broadcast(
				TEXT("Unable to prepare room search")
			);
		}
	}
}

void UMultiplayerSessionSubsystem::Deinitialize()
{
	if (SessionInterface.IsValid())
	{
		if (CreateSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
				CreateSessionCompleteDelegateHandle
			);

			CreateSessionCompleteDelegateHandle.Reset();
		}

		if (FindSessionsCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
				FindSessionsCompleteDelegateHandle
			);

			FindSessionsCompleteDelegateHandle.Reset();
		}

		if (DestroySessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(
				DestroySessionCompleteDelegateHandle
			);

			DestroySessionCompleteDelegateHandle.Reset();
		}
		if (JoinSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
				JoinSessionCompleteDelegateHandle
			);

			JoinSessionCompleteDelegateHandle.Reset();
		}
	}

	SessionSearch.Reset();
	SessionInterface.Reset();

	Super::Deinitialize();
}