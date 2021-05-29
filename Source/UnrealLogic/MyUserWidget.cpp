// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "MYMACRO.h"

void UMyUserWidget::NativeConstruct()
{
	if (USlider* slider = Cast<USlider>(GetWidgetFromName("Slider_17")))
	{
		
		slider->OnValueChanged.AddDynamic(this,&UMyUserWidget::OnValueChanged);
		
	}
}

void UMyUserWidget::OnValueChanged(float value)
{
	sliderValue=value;
	// LOGWARNING("滑条 %f",value)
}

void UMyUserWidget::SetRecord(int maxvalue)
{
	if (USlider* slider = Cast<USlider>(GetWidgetFromName("Slider_17")))
	{
		
		slider->SetMaxValue(maxvalue);
		
	}
}
