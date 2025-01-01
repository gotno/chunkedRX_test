#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "OSCMessage.h"
#include "OSCAddress.h"

#include "RxChunkGameMode.generated.h"

class UOSCClient;
class UOSCServer;
class UChunked;
class UTexture2D;

UCLASS()
class RXCHUNK_API ARxChunkGameMode : public AGameModeBase {
	GENERATED_BODY()
	
public:
	ARxChunkGameMode();

protected:
  void BeginPlay() override;

private:
  UPROPERTY()
  UOSCClient* osctx;
  UPROPERTY()
  UOSCServer* oscrx;

  UPROPERTY()
  TMap<int32, UChunked*> ChunkedSends;
  UPROPERTY()
  TMap<int32, UTexture2D*> Textures;

  void AddRoute(const FString& AddressPattern, const FName& MethodName);
  void AckChunk(
    const FOSCMessage &message,
    int32_t& chunkedId,
    int32_t& chunkNum
  );

  // receives
  UFUNCTION()
  void Echo(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void BlobTest(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void ChunkedTest(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void ChunkedTexture(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void TestMessage(
    const FOSCMessage& InMessage,
    const FString& InIPAddress,
    int32 InPort
  );

  // sends
  void SendEcho();
  void SendChunkAck(int32_t chunkedId, int32_t chunkNum);
};
