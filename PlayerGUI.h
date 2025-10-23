#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::AudioSource
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    PlayerAudio playerAudio;

    juce::TextButton loadButton{ "load" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "||" };
    juce::TextButton playButton{ "play" };
    juce::TextButton muteButton{ "mute"};
    
    juce::TextButton forwardButton{ ">>10s" };
    juce::TextButton rewindButton{ "<<10s" };


    juce::ToggleButton loopButton;  

    juce::Slider volumeSlider;
    juce::Slider speedSlider;

    bool isMuted;
    float savedGain;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};