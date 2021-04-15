// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DetectionNode.h"
#include "Components/Image.h"

void UDetectionNode::NativeConstruct() {
	Super::NativeConstruct();
}

UImage* UDetectionNode::GetFilling() {
	return Filling;
}

UWidgetAnimation* UDetectionNode::GetNodeUse()
{
	return NodeUse;
}
