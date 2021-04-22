// Fill out your copyright notice in the Description page of Project Settings.


#include "LinkObject.h"

void ULinkObject::init(TArray<FVector> t, TArray<ALinkStaticMeshActor*> t2, UNodeStaticMeshComponent* f,
                       UNodeStaticMeshComponent* t0)
{
	linkingPosition = t;
	linkingMesh = t2;
	from = f;
	to = t0;
}
