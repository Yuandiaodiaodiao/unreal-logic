// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "LinkStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class UNREALLOGIC_API ALinkStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()
	ALinkStaticMeshActor();
public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Circle;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Circle2;
	virtual void SetActorScale3D(FVector NewScale3D);


};
