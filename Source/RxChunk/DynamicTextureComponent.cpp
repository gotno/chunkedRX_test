#include "DynamicTextureComponent.h"

UDynamicTextureComponent::UDynamicTextureComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UDynamicTextureComponent::BeginPlay() {
	Super::BeginPlay();
}

void UDynamicTextureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
