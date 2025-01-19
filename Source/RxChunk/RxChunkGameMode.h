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
class AFaced;

UCLASS()
class RXCHUNK_API ARxChunkGameMode : public AGameModeBase {
	GENERATED_BODY()
	
public:
	ARxChunkGameMode();

protected:
  void BeginPlay() override;

private:
  AFaced* DemoActor;

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
  void rxEcho(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void rxBlobTest(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void rxChunkedTest(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void rxChunkedTexture(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void rxModuleInfo(
    const FOSCAddress& AddressPattern,
    const FOSCMessage &message,
    const FString &ipaddress,
    int32 port
  );
  UFUNCTION()
  void TestMessageReceipt(
    const FOSCMessage& InMessage,
    const FString& InIPAddress,
    int32 InPort
  );

  // sends
  void txEcho();
  void txChunkAck(int32_t chunkedId, int32_t chunkNum);
};
