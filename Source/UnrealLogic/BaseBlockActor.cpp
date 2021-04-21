// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBlockActor.h"



#include "CameraController.h"
#include "EditorPlayerController.h"
#include "NodeStaticMeshComponent.h"

// Sets default values
ABaseBlockActor::ABaseBlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// UNodeStaticMeshComponent* component=CreateDefaultSubobject<UNodeStaticMeshComponent>(TEXT("0"));
	// component->SetupAttachment(RootComponent);
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> circleMesh(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
	// component->SetStaticMesh(circleMesh.Object);
}


// Called when the game starts or when spawned
void ABaseBlockActor::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActorComponent*> nodeComponents= GetComponentsByClass(UNodeStaticMeshComponent::StaticClass());
	for(UActorComponent* componento : nodeComponents)
	{
		UNodeStaticMeshComponent* component=Cast<UNodeStaticMeshComponent>(componento);
		component->OnClicked.AddDynamic(this,&ABaseBlockActor::NodeClicked);
		
		// component->SetRelativeLocation(FVector(0,0,0));
	}
	UE_LOG(LogTemp,Warning,TEXT("begin"));

}

// Called every frame
void ABaseBlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBlockActor::NotifyActorOnClicked(FKey ButtonPressed)
{
		UE_LOG(LogTemp,Warning,TEXT("onclick"));
}

void ABaseBlockActor::NodeClicked(UPrimitiveComponent* component,FKey key)
{
	UE_LOG(LogTemp,Warning,TEXT("component clicked"));
	//拿到controller 通知
	auto playercontroller=GetWorld()->GetFirstPlayerController();
	auto controller=Cast<AEditorPlayerController>(playercontroller);
	auto componentNode=Cast<UNodeStaticMeshComponent>(component);
	controller->ChangeState(componentNode);
	
}

