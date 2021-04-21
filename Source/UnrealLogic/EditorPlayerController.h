// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "NodeStaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "EditorPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALLOGIC_API AEditorPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FString state = "";
	void ChangeState(UNodeStaticMeshComponent* component) ;
};
