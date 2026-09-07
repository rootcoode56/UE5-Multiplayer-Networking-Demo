// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicatedPickup.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AReplicatedPickup::AReplicatedPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("SceneRoot")
	);

	SetRootComponent(SceneRoot);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("PickupMesh")
	);

	PickupMesh->SetupAttachment(SceneRoot);
}

void AReplicatedPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AReplicatedPickup::CollectPickup()
{
	if (!HasAuthority())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CollectPickup rejected: caller does not have authority")
		);

		return;
	}

	Destroy();
}

