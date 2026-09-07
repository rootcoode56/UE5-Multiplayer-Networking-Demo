// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplicatedSwitch.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class MULTIPLAYERDEMO_API AReplicatedSwitch : public AActor
{
	GENERATED_BODY()

public:
	AReplicatedSwitch();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	void ToggleSwitch();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Switch"
	)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Switch"
	)
	TObjectPtr<UStaticMeshComponent> SwitchMesh;

	UPROPERTY(
		ReplicatedUsing = OnRep_IsActive,
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Switch"
	)
	bool bIsActive = false;

	UFUNCTION()
	void OnRep_IsActive();

private:
	void UpdateSwitchVisual();
};
