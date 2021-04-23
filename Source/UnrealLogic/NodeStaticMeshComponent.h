// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/StaticMeshComponent.h"
#include "NodeStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNREALLOGIC_API UNodeStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	UNodeStaticMeshComponent();
public:
	
	UPROPERTY(EditAnywhere)
	FString type = "input";
	UPROPERTY(VisibleAnywhere)
	TArray<UObject*> lineArray;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool nowactivate=false;
	bool nextactivate=false;
	
};
