#include <oboe/Oboe.h>
#include <cmath>
#include <cstring>
#include <atomic>
#include "SoundTouch.h"

using namespace oboe;
using namespace soundtouch;

class VoiceEngine : public AudioStreamDataCallback {
public:
    std::atomic<int> currentEffect{0};

    void start() {
        soundTouch.setSampleRate(48000);
        soundTouch.setChannels(1);
        updateEffectParameters();

        AudioStreamBuilder outBuilder;
        outBuilder.setDirection(Direction::Output)
                  ->setPerformanceMode(PerformanceMode::LowLatency)
                  ->setFormat(AudioFormat::Float)
                  ->setChannelCount(ChannelCount::Mono)
                  ->setSampleRate(48000)
                  ->setDataCallback(this)
                  ->openStream(outStream);

        AudioStreamBuilder inBuilder;
        inBuilder.setDirection(Direction::Input)
                 ->setPerformanceMode(PerformanceMode::LowLatency)
                 ->setFormat(AudioFormat::Float)
                 ->setChannelCount(ChannelCount::Mono)
                 ->setSampleRate(48000)
                 ->openStream(inStream);

        if(inStream) inStream->requestStart();
        if(outStream) outStream->requestStart();
    }

    void setEffect(int effectId) {
        currentEffect.store(effectId);
        updateEffectParameters();
    }

    DataCallbackResult onAudioReady(AudioStream *stream, void *audioData, int32_t numFrames) override {
        float *output = static_cast<float *>(audioData);
        if (!inStream) {
            memset(output, 0, numFrames * sizeof(float));
            return DataCallbackResult::Continue;
        }
        float inputBuffer[numFrames];
        auto result = inStream->read(inputBuffer, numFrames, 0);
        if (result != Result::OK || result.value() == 0) {
            memset(output, 0, numFrames * sizeof(float));
            return DataCallbackResult::Continue;
        }

        int effect = currentEffect.load();
        if (effect == 0) {
            memcpy(output, inputBuffer, numFrames * sizeof(float));
        } else if (effect == 3) {
            for (int i = 0; i < numFrames; ++i) {
                float carrier = sinf(2.0f * M_PI * 40.0f * phase / 48000.0f);
                output[i] = inputBuffer[i] * carrier * 1.5f;
                phase = (phase + 1) % 48000;
            }
        } else {
            soundTouch.putSamples(inputBuffer, numFrames);
            uint received = soundTouch.receiveSamples(output, numFrames);
            if (received < (uint)numFrames) {
                memset(output + received, 0, (numFrames - received) * sizeof(float));
            }
        }
        return DataCallbackResult::Continue;
    }

private:
    std::shared_ptr<AudioStream> inStream;
    std::shared_ptr<AudioStream> outStream;
    SoundTouch soundTouch;
    int phase = 0;

    void updateEffectParameters() {
        soundTouch.clear();
        switch (currentEffect.load()) {
            case 1: soundTouch.setPitchSemiTones(10.0f); break; // Ardilla
            case 2: soundTouch.setPitchSemiTones(7.5f); break;  // Helio
            case 4: soundTouch.setPitchSemiTones(-7.0f); break; // Monstruo
            default: soundTouch.setPitchSemiTones(0.0f); break;
        }
    }
};
