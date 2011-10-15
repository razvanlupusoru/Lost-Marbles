#pragma once

namespace Ogre {
class TextureUnitState;
class Overlay;
}

class OverlayFaderCallback {
public:
    virtual void fadeInCallback(void) {}
    virtual void fadeOutCallback(void) {}
};

class OverlayFader {
public:
    OverlayFader(Ogre::Overlay* overlay, const char *MaterialName, Ogre::Overlay* overlay2, const char *MaterialName2,OverlayFaderCallback *instance = 0);
    ~OverlayFader(void);

    void startFadeIn(double duration = 1.0f);
    void startFadeOut(double duration = 1.0f);
    void fade(double timeSinceLastFrame);

protected:
    double _alpha;
    double _current_dur;
    double _total_dur;
    OverlayFaderCallback *_inst;
    Ogre::TextureUnitState *_tex_unit;
    Ogre::TextureUnitState *_tex_unit2;
    Ogre::Overlay *_overlay;
    Ogre::Overlay* _overlay2;


    enum _fadeop {
        FADE_NONE,
        FADE_IN,
        FADE_OUT,
    } _fadeop;
};
