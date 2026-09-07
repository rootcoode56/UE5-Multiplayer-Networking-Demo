// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPickupCountChanged,
	int32,
	NewPickupCount
);

UCLASS()
class MULTIPLAYERDEMO_API AMultiplayerPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMultiplayerPlayerState();

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnPickupCountChanged OnPickupCountChanged;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	void AddPickup();

	int32 GetPickupCount() const
	{
		return PickupCount;
	}

protected:
	UPROPERTY(
		ReplicatedUsing = OnRep_PickupCount,
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Pickup"
	)
	int32 PickupCount = 0;

	UFUNCTION()
	void OnRep_PickupCount();
};
