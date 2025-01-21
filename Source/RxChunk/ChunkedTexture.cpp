#include "ChunkedTexture.h"

#define QOI_IMPLEMENTATION
#include "ThirdParty/qoi/qoi.h"

#include "Engine/Texture2D.h"

void UChunkedTexture::SetDimensions(int32_t inWidth, int32_t inHeight) {
  Width = inWidth;
  Height = inHeight;
}

void UChunkedTexture::SetCompressed(bool inbCompressed) {
  bCompressed = inbCompressed;
}

UTexture2D* UChunkedTexture::MakeTexture2D() {
  UTexture2D* TextureTarget =
    UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

  void* MipData =
    TextureTarget->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

  FMemory::Memcpy(MipData, Data.GetData(), Width * Height * 4);

  TextureTarget->GetPlatformData()->Mips[0].BulkData.Unlock();
  TextureTarget->UpdateResource();

  return TextureTarget;
}

void UChunkedTexture::Decompress() {
  if (!bCompressed) return;

  qoi_desc filedesc;
  uint8_t* decompressed =
    (uint8_t*)qoi_decode(Data.GetData(), Data.Num(), &filedesc, 4);

  int32_t size = filedesc.width * filedesc.height * filedesc.channels;

  Data.Empty();
  Data.SetNum(size);
  FMemory::Memcpy(Data.GetData(), decompressed, size);

  bCompressed = false;
}

void UChunkedTexture::Finish() {
  Decompress();
  OnComplete.Broadcast(this);
}
