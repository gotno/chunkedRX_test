#pragma once

#include "CoreMinimal.h"
#include "Chunked.h"
#include "ChunkedTexture.generated.h"

class UTexture2D;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChunkedTextureCompleteDelegate, uint32_t /* id */, UTexture2D* /* Texture */);

UCLASS()
class RXCHUNK_API UChunkedTexture : public UChunked {
	GENERATED_BODY()
	
public:
  void SetDimensions(int32_t inWidth, int32_t inHeight);
  void SetCompressed(bool inbCompressed);

  FOnChunkedTextureCompleteDelegate OnComplete;

private:
  int32_t Width{0}, Height{0};
  bool bCompressed{false};

  UTexture2D* MakeTexture();
  void Finish() override;
};