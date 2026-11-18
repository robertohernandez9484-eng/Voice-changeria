#include <jni.h>
#include <memory>
#include "VoiceEngine.cpp"

static std::unique_ptr<VoiceEngine> engine = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_tudominio_voicetuner_GameTurboOverlayService_startVoiceEngine(JNIEnv* env, jobject) {
    if (!engine) engine = std::make_unique<VoiceEngine>();
    engine->start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_tudominio_voicetuner_GameTurboOverlayService_setVoiceEffect(JNIEnv* env, jobject, jint effectId) {
    if (engine) engine->setEffect(static_cast<int>(effectId));
}
