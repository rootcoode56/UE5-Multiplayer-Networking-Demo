// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FMultiplayerOnCreateSessionComplete,
	bool,
	bWasSuccessful
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnRoomJoinFailed,
	const FString&,
	ErrorMessage
);

UCLASS()
class MULTIPLAYERDEMO_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer|Session")
	FOnRoomJoinFailed OnRoomJoinFailed;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(BlueprintReadOnly)
	FString CurrentRoomCode;

	FString DesiredRoomCode;

	bool bCreateSessionOnDestroy = false;
	bool bFindSessionsOnDestroy = false;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	UPROPERTY(BlueprintAssignable)
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION(BlueprintCallable)
	void JoinSessionByRoomCode(const FString& RoomCode);

	FString GenerateRoomCode();

	void OnCreateSessionComplete(
		FName SessionName,
		bool bWasSuccessful
	);

	void FindSessions();

	void OnFindSessionsComplete(
		bool bWasSuccessful
	);

	void OnJoinSessionComplete(
		FName SessionName,
		EOnJoinSessionCompleteResult::Type Result
	);

	void DestroySession();

	void OnDestroySessionComplete(
		FName SessionName,
		bool bWasSuccessful
	);
};