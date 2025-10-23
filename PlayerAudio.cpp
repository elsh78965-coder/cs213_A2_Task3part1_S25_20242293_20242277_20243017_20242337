#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();

    resampler.reset(new juce::ResamplingAudioSource(&transportSource, false, 2));
}

PlayerAudio::~PlayerAudio()
{
    stop();
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (resampler) resampler->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (resampler)
        resampler->getNextAudioBlock(bufferToFill);
    else
        transportSource.getNextAudioBlock(bufferToFill);

    const double length = transportSource.getLengthInSeconds();
    if (length > 0.0)
    {
        const double pos = transportSource.getCurrentPosition();
        const double epsilon = 0.02; 

        if (pos + epsilon >= length)
        {
            if (transportSource.isLooping() || userLooping)
            {
                transportSource.setPosition(0.0);
                transportSource.start();
                DBG("PlayerAudio: fallback loop restart executed (userLooping=" << (userLooping ? "YES" : "NO") << ")");
            }
        }
    }
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    transportSource.setLooping(shouldLoop);
    userLooping = shouldLoop;
    DBG("PlayerAudio::setLooping called -> " << (shouldLoop ? "ON" : "OFF"));
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    if (resampler) resampler->releaseResources();
    readerSource.reset();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return false;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        return true;
    }

    return false;
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}


bool PlayerAudio::isLooping() const
{
    return transportSource.isLooping();
}

void PlayerAudio::setSpeed(float ratio)
{
    if (resampler)
    {
        resampler->setResamplingRatio(ratio);
    }
}