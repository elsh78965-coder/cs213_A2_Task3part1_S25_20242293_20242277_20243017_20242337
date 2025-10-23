

//link of Github>>https://github.com/elsh78965-coder/project.git


#include "PlayerGUI.h"
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkturquoise);
}

PlayerGUI::PlayerGUI()
{
    // ===== الأزرار الأساسية =====
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &playButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Speed slider
    speedSlider.setRange(0.25, 2.0, 0.25);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    // Mute button
    addAndMakeVisible(muteButton);
    muteButton.addListener(this);
    isMuted = false;
    savedGain = (float)volumeSlider.getValue();

    // Play button
    playButton.setButtonText("play");
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    addAndMakeVisible(playButton);

    playButton.repaint();

    // ===== Loop Button =====
    loopButton.setButtonText("Loop:Off");
    loopButton.setClickingTogglesState(true);  
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    for (auto* btn : { &forwardButton, &rewindButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }



    // ================= clours =================

// Load (blue)
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blueviolet);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // Restart (mediumorchid)
    restartButton.setColour(juce::TextButton::buttonColourId, juce::Colours::mediumorchid);
    restartButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // Stop (Red)
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // Play (green)
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // mute(orang)
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
    muteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    // Loop 
    loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    loopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);


    

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    playButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    loopButton.setBounds(520, y, 80, 40);
    forwardButton.setBounds(720, 20, 80, 40);
    rewindButton.setBounds(620, 20, 80, 40);


    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
    speedSlider.setBounds(20, 150, getWidth() - 40, 30);
}

PlayerGUI::~PlayerGUI()
{
    loadButton.removeListener(this);
    restartButton.removeListener(this);
    stopButton.removeListener(this);
    playButton.removeListener(this);
    muteButton.removeListener(this);
    loopButton.removeListener(this);

    volumeSlider.removeListener(this);
    speedSlider.removeListener(this);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                }
            });
    }

    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }

    else if (button == &stopButton)
    {
        playerAudio.stop();
    }

    else if (button == &playButton)
    {
        playerAudio.start();
    }

    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            savedGain = (float)volumeSlider.getValue();
            playerAudio.setGain(0.0f);
            isMuted = true;
            muteButton.setButtonText("unmute");
        }
        else
        {
            playerAudio.setGain(savedGain);
            volumeSlider.setValue(savedGain, juce::dontSendNotification);
            isMuted = false;
            muteButton.setButtonText("mute");
        }
    }

    else if (button == &loopButton)
    {
        bool shouldLoop = loopButton.getToggleState();
        DBG("Loop button clicked. toggle state = " << (shouldLoop ? "ON" : "OFF"));

        playerAudio.setLooping(shouldLoop);

        DBG("After setLooping: playerAudio.isLooping() = " << (playerAudio.isLooping() ? "YES" : "NO"));
        DBG("current position = " << playerAudio.getPosition());
        DBG("length = " << playerAudio.getLength());

        loopButton.setButtonText(shouldLoop ? "Loop:On" : "Loop:Off");
    }
    else if (button == &forwardButton)
    {
        double newPos = playerAudio.getPosition() + 10.0;
        if (newPos > playerAudio.getLength())
            newPos = playerAudio.getLength(); 
        playerAudio.setPosition(newPos);
    }

    else if (button == &rewindButton)
    {
        double newPos = playerAudio.getPosition() - 10.0;
        if (newPos < 0.0)
            newPos = 0.0;
        playerAudio.setPosition(newPos);
    }

}


void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    else if (slider == &speedSlider)
    {
        playerAudio.setSpeed((float)slider->getValue());
    }
}