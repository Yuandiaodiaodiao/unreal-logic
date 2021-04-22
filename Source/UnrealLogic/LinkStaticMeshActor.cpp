// Fill out your copyright notice in the Description page of Project Settings.


#include "LinkStaticMeshActor.h"

ALinkStaticMeshActor::ALinkStaticMeshActor()
{
	static auto mesh = ConstructorHelpers::FObjectFinderOptional<UStaticMesh>(
         TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'")).Get();
	auto meshComponent=Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	
	meshComponent->SetStaticMesh(mesh);
	static auto material=ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Chrome.M_Metal_Chrome'")).Get();
	meshComponent->SetMaterial(0,material);
	// meshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	meshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SetMobility(EComponentMobility::Movable);
	auto bound=meshComponent->Bounds;
	auto x=bound.BoxExtent;
	// UE_LOG(LogTemp,Warning,TEXT("%s"),*x.ToString());
	auto bound2=meshComponent->Bounds;
	auto meshComponent1=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("circle1"));
	Circle=meshComponent1;
	static auto mesh1 = ConstructorHelpers::FObjectFinderOptional<UStaticMesh>(
         TEXT("StaticMesh'/Game/StarterContent/Props/MaterialSphere.MaterialSphere'")).Get();
	meshComponent1->SetStaticMesh(mesh1);
	meshComponent1->SetMaterial(0,material);
	meshComponent1->SetRelativeLocation(FVector(0,0,50));
	meshComponent1->SetWorldScale3D(FVector(0.25,0.25,0.25));
	meshComponent1->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	meshComponent1->AttachToComponent(meshComponent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
			EAttachmentRule::KeepWorld,true),FName("0"));

	auto meshComponent2=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("circle2"));
	Circle2=meshComponent2;

	meshComponent2->SetStaticMesh(mesh1);
	meshComponent2->SetMaterial(0,material);
	meshComponent2->SetRelativeLocation(FVector(0,0,-50));
	meshComponent2->SetWorldScale3D(FVector(0.25,0.25,0.25));
	meshComponent2->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	meshComponent2->AttachToComponent(meshComponent,
        FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,
            EAttachmentRule::KeepWorld,true),FName("1"));
	
}

void ALinkStaticMeshActor::SetActorScale3D(FVector NewScale3D)
{
	Super::SetActorScale3D(NewScale3D);
	Circle->SetWorldScale3D(FVector(0.25f,0.25f,0.25f));
	Circle2->SetWorldScale3D(FVector(0.25f,0.25f,0.25f));
}
