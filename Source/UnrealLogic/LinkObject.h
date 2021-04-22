// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LinkStaticMeshActor.h"
#include "NodeStaticMeshComponent.h"
#include "UObject/NoExportTypes.h"
#include "LinkObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNREALLOGIC_API ULinkObject : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> linkingPosition;
	UPROPERTY(VisibleAnywhere)
	TArray<ALinkStaticMeshActor*> linkingMesh;
	UPROPERTY(VisibleAnywhere)
	UNodeStaticMeshComponent* from;
	UPROPERTY(VisibleAnywhere)
	UNodeStaticMeshComponent* to;
	void init(TArray<FVector> t, TArray<ALinkStaticMeshActor*> t2, UNodeStaticMeshComponent* f,
	          UNodeStaticMeshComponent* t0);
};
