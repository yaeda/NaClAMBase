#include "NaClAMBase/NaClAMBase.h"
#include "floatops.h"

/**
 * This function is called at module initialization time.
 * moduleInterfaces and moduleInstance are already initialized.
 */
void NaClAMModuleInit() {
  NaClAMPrintf("AM Running.");
  NaClAMPrintf("Build %s %s", __DATE__, __TIME__);
}

/**
 * This function is called at 60hz.
 * @param microseconds A monotonically increasing clock
 */
void NaClAMModuleHeartBeat(uint64_t microseconds) {
}

void handleFloatSum(const NaClAMMessage& message) {
  float sum = 0.0;
  if (message.frames[0].type == PP_VARTYPE_ARRAY_BUFFER) {
    // Map array
    uint32_t len = 0;
    moduleInterfaces.varArrayBuffer->ByteLength(message.frames[0], &len);
    float* buf = (float*)moduleInterfaces.varArrayBuffer->Map(message.frames[0]);
    uint32_t numFloats = len/sizeof(float);
    // Do work
    sum = sumFloatArray(buf, numFloats);
    // Unmap array
    moduleInterfaces.varArrayBuffer->Unmap(message.frames[0]);
  }

  // Send reply
  {
    Json::Value reply = NaClAMMakeReplyObject("floatsum", message.requestId);
    reply["sum"] = Json::Value(sum);
    NaClAMSendMessage(reply, NULL, 0);
  }
}

void handleSubFloats(const NaClAMMessage& message) {
  if (message.frames[0].type == PP_VARTYPE_ARRAY_BUFFER &&
      message.frames[1].type == PP_VARTYPE_ARRAY_BUFFER) {
        // Map arrays
        uint32_t len1 = 0;
        uint32_t len2 = 0;
        moduleInterfaces.varArrayBuffer->ByteLength(message.frames[0], &len1);
        moduleInterfaces.varArrayBuffer->ByteLength(message.frames[1], &len2);
        float* buf1 = (float*)moduleInterfaces.varArrayBuffer->Map(message.frames[0]);
        float* buf2 = (float*)moduleInterfaces.varArrayBuffer->Map(message.frames[1]);
        uint32_t numFloats = len1 < len2 ? len1 : len2;
        numFloats = numFloats/sizeof(float);
        // Do work
        subFloatArrays(buf1, buf2, numFloats);
        // Unmap arrays
        moduleInterfaces.varArrayBuffer->Unmap(message.frames[0]);
        moduleInterfaces.varArrayBuffer->Unmap(message.frames[1]);
  }

  // Send reply
  {
    Json::Value reply = NaClAMMakeReplyObject("floatsub", message.requestId);
    NaClAMSendMessage(reply, &message.frames[0], 1);
  }
}

void handleAddFloats(const NaClAMMessage& message) {
  if (message.frames[0].type == PP_VARTYPE_ARRAY_BUFFER &&
    message.frames[1].type == PP_VARTYPE_ARRAY_BUFFER) {
      // Map arrays
      uint32_t len1 = 0;
      uint32_t len2 = 0;
      moduleInterfaces.varArrayBuffer->ByteLength(message.frames[0], &len1);
      moduleInterfaces.varArrayBuffer->ByteLength(message.frames[1], &len2);
      float* buf1 = (float*)moduleInterfaces.varArrayBuffer->Map(message.frames[0]);
      float* buf2 = (float*)moduleInterfaces.varArrayBuffer->Map(message.frames[1]);
      uint32_t numFloats = len1 < len2 ? len1 : len2;
      numFloats = numFloats/sizeof(float);
      // Do work
      addFloatArrays(buf1, buf2, numFloats);
      // Unmap arrays
      moduleInterfaces.varArrayBuffer->Unmap(message.frames[0]);
      moduleInterfaces.varArrayBuffer->Unmap(message.frames[1]);
  }

  // Send reply
  {
    Json::Value reply = NaClAMMakeReplyObject("floatadd", message.requestId);
    NaClAMSendMessage(reply, &message.frames[0], 1);
  }
}

/**
 * This function is called for each message received from JS
 * @param message A complete message sent from JS
 */
void NaClAMModuleHandleMessage(const NaClAMMessage& message) {
  if (message.cmdString.compare("floatsum") == 0) {
    handleFloatSum(message);
  } else if (message.cmdString.compare("subfloatarrays") == 0) {
    handleSubFloats(message);
  } else if (message.cmdString.compare("addfloatarrays") == 0) {
    handleAddFloats(message);
  } else {
    NaClAMPrintf("Got message I don't understand: %s", message.cmdString.c_str());
  }
}