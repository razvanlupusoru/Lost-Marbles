#include "TextRenderer.h"

template<> TextRenderer* Ogre::Singleton<TextRenderer>::ms_Singleton = 0;


TextRenderer::TextRenderer()
{
	_overlayMgr = Ogre::OverlayManager::getSingletonPtr();

	_overlay = _overlayMgr->create("overlay1");
	_panel = static_cast<Ogre::OverlayContainer*>(_overlayMgr->createOverlayElement("Panel", "container1"));
	_panel->setDimensions(1, 1);
	_panel->setPosition(0, 0);

	_overlay->add2D(_panel);

	_overlay->show();
}

void TextRenderer::addTextBox(const std::string& ID,
				const std::string& text,
				Ogre::Real x, Ogre::Real y,
				Ogre::Real width, Ogre::Real height,
				const Ogre::ColourValue& color,
				std::string size)
{
	Ogre::OverlayElement* textBox = _overlayMgr->createOverlayElement("TextArea", ID);
	textBox->setDimensions(width, height);
	textBox->setMetricsMode(Ogre::GMM_PIXELS);
	textBox->setPosition(x, y);
	textBox->setWidth(width);
	textBox->setHeight(height);
	if(atoi(size.c_str())>=72)
		textBox->setParameter("font_name", "BlueHighway72");
	else
		textBox->setParameter("font_name", "BlueHighway");
	textBox->setParameter("char_height", size);
	textBox->setColour(color);

	textBox->setCaption(text);

	_panel->addChild(textBox);
}

void TextRenderer::removeTextBox(const std::string& ID)
{
	_panel->removeChild(ID);
	_overlayMgr->destroyOverlayElement(ID);
}

void TextRenderer::removeAllTextBoxes()
{
	Ogre::OverlayContainer::ChildIterator ite = _panel->getChildIterator();
	while(ite.hasMoreElements()){
		Ogre::OverlayElement *child = ite.getNext();
		_panel->removeChild(child->getName());
		_overlayMgr->destroyOverlayElement(child->getName());
		//ite.moveNext();
	}
}

void TextRenderer::setText(const std::string& ID, const std::string& Text)
{
	Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
	textBox->setCaption(Text);
}

