#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicTextureComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RXCHUNK_API UDynamicTextureComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	UDynamicTextureComponent();

  void Init(const uint32_t& inWidth, const uint32_t& inHeight);
  bool IsInitialized();
  void SetData(TArray<uint8_t>& inData);
  UTexture2D* GetTexture();

private:
  UPROPERTY()
  UTexture2D* Texture;

  uint32_t Width{0}, Height{0}, BitsPerPixel{4};
  TArray<uint8_t> Data;

  bool bIsInitialized{false};
  void UpdateTexture();
};
