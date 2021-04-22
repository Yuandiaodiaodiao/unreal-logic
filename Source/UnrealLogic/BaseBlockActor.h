// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBlockActor.generated.h"

UCLASS()
class UNREALLOGIC_API ABaseBlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBlockActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	UFUNCTION()
	void NodeClicked(UPrimitiveComponent* component,FKey key);
	
	UPROPERTY(EditAnywhere)
	FString GateType;
};
