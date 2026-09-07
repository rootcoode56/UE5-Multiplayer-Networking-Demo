// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplicatedPickup.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class MULTIPLAYERDEMO_API AReplicatedPickup : public AActor
{
	GENERATED_BODY()

public:
	AReplicatedPickup();

	void CollectPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Pickup"
	)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Pickup"
	)
	TObjectPtr<UStaticMeshComponent> PickupMesh;
};
