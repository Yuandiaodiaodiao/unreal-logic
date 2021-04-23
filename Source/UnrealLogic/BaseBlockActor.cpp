// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBlockActor.h"


#include "CameraController.h"
#include "EditorPlayerController.h"
#include "EditorStateBase.h"
#include "NodeStaticMeshComponent.h"

// Sets default values
ABaseBlockActor::ABaseBlockActor()
{

	materialInActivate=ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/StarterContent/Materials/M_Water_Lake.M_Water_Lake'")).Get();
	materialActivate=ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/Blueprints/Green.Green'")).Get();
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
	TArray<UActorComponent*> nodeComponents = GetComponentsByClass(UNodeStaticMeshComponent::StaticClass());
	for (UActorComponent* componento : nodeComponents)
	{
		UNodeStaticMeshComponent* component = Cast<UNodeStaticMeshComponent>(componento);
		component->OnClicked.AddUniqueDynamic(this, &ABaseBlockActor::NodeClicked);


		// component->SetRelativeLocation(FVector(0,0,0));
	}
	auto gameState=Cast<AEditorStateBase>(GetWorld()->GetGameState());
	gameState->blockArray.Add(this);
	// UE_LOG(LogTemp, Warning, TEXT("begin"));
}



// Called every frame
void ABaseBlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// TArray<UNodeStaticMeshComponent*, TInlineAllocator<16>> componentArray;
	// GetComponents<UNodeStaticMeshComponent, TInlineAllocator<16>>(componentArray);
	//
	// // UMeshComponent::SetMaterial(0,material);
	// for (auto& component : componentArray)
	// {
	// 	
	// 	if(component->nowactivate)
	// 	{
	// 		UE_LOG(LogTemp,Warning,TEXT("添加材质"));
	// 		component->SetMaterial(0,materialActivate);
	// 	}else
	// 	{
	// 		component->SetMaterial(0,materialInActivate);
	// 	}
	// 	
	// }
}

void ABaseBlockActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	TArray<UActorComponent*> nodeComponents = GetComponentsByClass(UNodeStaticMeshComponent::StaticClass());
	for (UActorComponent* componento : nodeComponents)
	{
		UNodeStaticMeshComponent* component = Cast<UNodeStaticMeshComponent>(componento);
		component->OnClicked.AddUniqueDynamic(this, &ABaseBlockActor::NodeClicked);

		// component->SetRelativeLocation(FVector(0,0,0));
	}
	UE_LOG(LogTemp, Warning, TEXT("Actor onclick"));
}

void ABaseBlockActor::NodeClicked(UPrimitiveComponent* component, FKey key)
{
	UE_LOG(LogTemp, Warning, TEXT("component clicked"));
	//拿到controller 通知
	auto playercontroller = GetWorld()->GetFirstPlayerController();
	auto controller = Cast<AEditorPlayerController>(playercontroller);
	auto componentNode = Cast<UNodeStaticMeshComponent>(component);
	FVector componentLocation = componentNode->GetComponentTransform().GetLocation();
	// FVector actorLocation=((AActor*)componentNode->GetOuter())->GetActorLocation();
	FVector actorLocation = componentNode->GetAttachParent()->GetComponentLocation();
	componentNode->GetAttachParent()->GetName();
	FVector direct = actorLocation - componentLocation;
	UE_LOG(LogTemp, Warning, TEXT("%s %s &s"), *componentLocation.ToString(), *actorLocation.ToString(),
	       *componentNode->GetAttachParent()->GetName());
	direct = direct.GetAbs();
	if (direct.X > direct.Y)
	{
		direct = FVector(1, 0, 0);
	}
	else
	{
		direct = FVector(0, 1, 0);
	}
	controller->ChangeNodeState(componentNode, direct);
}
