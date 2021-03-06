// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseBlockActor.h"
#include "LinkStaticMeshActor.h"
#include "MyUserWidget.h"
#include "NodeStaticMeshComponent.h"
#include "Recorder.h"
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
	bool menuOn=false;
	UPROPERTY(EditAnywhere)
	bool simOn=false;
	bool recordOn=false;
	ABaseBlockActor* actorShow;
protected:
	TArray<UClass*> PutList;
	FVector lastEndPosition;
	TArray<FVector> linkingPositionTemp;
	TArray<ALinkStaticMeshActor*> linkingMeshTemp;
	Recorder* record;
	UMyUserWidget* WidgetInstance;
	bool rightMouseStatus=false;
	void OnMouseMove(FVector2D mousePosition);
	void ChangeLinkShape(FVector2D mousePosition);
	void MouseLeftClick();
	void MouseRightClick();
	void MouseRightRelease();
	void MenuOn();
	void Next(int idDelta);
	void Next();
	void ReFreshPut();
	void StartSim();
	void AlwaysSim();
	void SaveVerilog();
	void Refresh();
	void Record();
	void PerformanceTest();
	void ChangeMesh(FVector start, FVector end, ALinkStaticMeshActor* mesh);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	ALinkStaticMeshActor* createLinkingMesh(FVector startPosition);
	virtual void SetupInputComponent() override;

public:
	void ChangeNodeState(UNodeStaticMeshComponent* component, FVector startNormal);
};
