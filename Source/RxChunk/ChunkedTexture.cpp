#include "ChunkedTexture.h"

#define QOI_IMPLEMENTATION
#include "ThirdParty/qoi/qoi.h"

#include "Engine/Texture2D.h"

void UChunkedTexture::SetDimensions( int32_t inWidth, int32_t inHeight) {
  Width = inWidth;
  Height = inHeight;
}

UTexture2D* UChunkedTexture::MakeTexture() {
  qoi_desc filedesc;
  uint8* uncompressed =
    (uint8*)qoi_decode(Data.GetData(), Data.Num(), &filedesc, 4);

  UTexture2D* TextureTarget =
    UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

  uint8* MipData =
    (uint8*)TextureTarget->
      GetPlatformData()->
      Mips[0].BulkData.Lock(LOCK_READ_WRITE);

  FMemory::Memcpy(MipData, uncompressed, Width * Height * 4);

  TextureTarget->GetPlatformData()->Mips[0].BulkData.Unlock();
  TextureTarget->UpdateResource();

  free(uncompressed);
  return TextureTarget;
}

void UChunkedTexture::Finish() {
  OnComplete.Broadcast(id, MakeTexture());
}
