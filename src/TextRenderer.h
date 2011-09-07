#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <Ogre.h>
#include <OgreSingleton.h>

#include <vector>

class TextRenderer : public Ogre::Singleton<TextRenderer>
{
private:

	Ogre::OverlayManager*    _overlayMgr;
	Ogre::Overlay*           _overlay;
	Ogre::OverlayContainer*  _panel;

public:

	TextRenderer();
	~TextRenderer();

	void addTextBox(
		const std::string& ID,
		const std::string& text,
		Ogre::Real x, Ogre::Real y,
		Ogre::Real width, Ogre::Real height,
		const Ogre::ColourValue& color = Ogre::ColourValue(1.0, 1.0, 1.0, 1.0),
		std::string size="12");

	void removeTextBox(const std::string& ID);
	void removeAllTextBoxes();

	void setText(const std::string& ID, const std::string& Text);
};

#endif
