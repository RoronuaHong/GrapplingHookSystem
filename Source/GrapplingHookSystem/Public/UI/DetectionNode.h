// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionNode.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class GRAPPLINGHOOKSYSTEM_API UDetectionNode : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void NativeConstruct() override;

	UPROPERTY(Meta=(BindWidget))
	UImage* Background;

	UPROPERTY(Meta=(BindWidget))
	UImage* Filling;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* NodeUse;

public:
	UImage* GetFilling();

	UWidgetAnimation* GetNodeUse();
};
