// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "LinkStaticMeshActor.h"
#include "NodeStaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "EditorPlayerController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNREALLOGIC_API AEditorPlayerController : public APlayerController
{
	GENERATED_BODY()
	AEditorPlayerController();
	FString mouseState;
	FVector linkDirection;
	bool lock = false;

protected:
	TArray<UClass*> PutList;
	FVector lastEndPosition;
	TArray<FVector> linkingPositionTemp;
	TArray<ALinkStaticMeshActor*> linkingMeshTemp;
	void OnMouseMove(FVector2D mousePosition);
	void ChangeLinkShape(FVector2D mousePosition);
	void MouseLeftClick();
	void MouseRightClick();
	void MenuOn();
	void Next();
	void ChangeMesh(FVector start, FVector end, ALinkStaticMeshActor* mesh);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	ALinkStaticMeshActor* createLinkingMesh(FVector startPosition);
	virtual void SetupInputComponent() override;

public:
	void ChangeNodeState(UNodeStaticMeshComponent* component, FVector startNormal);
};
