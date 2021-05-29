// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALLOGIC_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

	private:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnValueChanged(float value);
	public:
	float sliderValue;
	void SetRecord(int maxvalue);
};
