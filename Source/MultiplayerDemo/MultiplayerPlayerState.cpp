// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerPlayerState.h"

#include "Net/UnrealNetwork.h"

AMultiplayerPlayerState::AMultiplayerPlayerState()
{
	PickupCount = 0;
}

void AMultiplayerPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(
		AMultiplayerPlayerState,
		PickupCount
	);
}

void AMultiplayerPlayerState::AddPickup()
{
	if (!HasAuthority())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("AddPickup rejected: PlayerState has no authority")
		);

		return;
	}

	++PickupCount;

	OnPickupCountChanged.Broadcast(PickupCount);

	ForceNetUpdate();
}

void AMultiplayerPlayerState::OnRep_PickupCount()
{
	OnPickupCountChanged.Broadcast(PickupCount);
}
