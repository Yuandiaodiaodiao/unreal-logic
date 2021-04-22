// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LinkObject.h"
#include "GameFramework/GameStateBase.h"
#include "EditorStateBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNREALLOGIC_API AEditorStateBase : public AGameStateBase
{
	GENERATED_BODY()
	public:
	UPROPERTY(VisibleAnywhere)
	TArray<ULinkObject*>lineArray;
};