//------------------------------------------------------------------------------
/*
  LayerEffects for JUCE

  Official project location:
  https://github.com/vinniefalco/LayerEffects

  ------------------------------------------------------------------------------

  License: MIT License (http://www.opensource.org/licenses/mit-license.php)
  Copyright (c) 2012 by Vinnie Falco

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
//------------------------------------------------------------------------------

CBlendModeResult::CBlendModeResult ()
  : m_blendMode (vf::BlendMode::normal)
{
  m_comboBox = new ComboBox ("Mode");
  m_comboBox->addItem ("Normal"       , vf::BlendMode::normal );
  m_comboBox->addItem ("Lighten"      , vf::BlendMode::lighten );
  m_comboBox->addItem ("Darken"       , vf::BlendMode::darken );
  m_comboBox->addItem ("Multiply"     , vf::BlendMode::multiply );
  m_comboBox->addItem ("Average"      , vf::BlendMode::average );
  m_comboBox->addItem ("Add"          , vf::BlendMode::add );
  m_comboBox->addItem ("Subtract"     , vf::BlendMode::subtract );
  m_comboBox->addItem ("Difference"   , vf::BlendMode::difference );
  m_comboBox->addItem ("Negation"     , vf::BlendMode::negation );
  m_comboBox->addItem ("Screen"       , vf::BlendMode::screen );
  m_comboBox->addItem ("Exclusion"    , vf::BlendMode::exclusion );
  m_comboBox->addItem ("Overlay"      , vf::BlendMode::overlay );
  m_comboBox->addItem ("Soft Light"   , vf::BlendMode::softLight );
  m_comboBox->addItem ("Hard Light"   , vf::BlendMode::hardLight );
  m_comboBox->addItem ("Color Dodge"  , vf::BlendMode::colorDodge );
  m_comboBox->addItem ("Color Burn"   , vf::BlendMode::colorBurn );
  m_comboBox->addItem ("Linear Dodge" , vf::BlendMode::linearDodge );
  m_comboBox->addItem ("Linear Burn"  , vf::BlendMode::linearBurn );
  m_comboBox->addItem ("Linear Light" , vf::BlendMode::linearLight );
  m_comboBox->addItem ("Vivid Light"  , vf::BlendMode::vividLight );
  m_comboBox->addItem ("Pin Light"    , vf::BlendMode::pinLight );
  m_comboBox->addItem ("Hard Mix"     , vf::BlendMode::hardMix );
  m_comboBox->addItem ("Reflect"      , vf::BlendMode::reflect );
  m_comboBox->addItem ("Glow"         , vf::BlendMode::glow );
  m_comboBox->addItem ("Phoenix"      , vf::BlendMode::phoenix );

  addAndMakeVisible (m_comboBox);

  m_comboBox->addListener (this);

  m_comboBox->setSelectedId (vf::BlendMode::normal);
}

CBlendModeResult::~CBlendModeResult()
{
}

void CBlendModeResult::setSourceImage (int index, Image image)
{
  m_sourceImage [index] = image;
  repaint ();
}

void CBlendModeResult::resized ()
{
  Rectangle <int> const bounds = getLocalBounds ();

  m_comboBox->setBounds (bounds.getX (), bounds.getY (), bounds.getWidth () - 24, 24);
}

void CBlendModeResult::paint (Graphics& g)
{
  Rectangle <int> const r (0, 24, 256, 256);

  vf::BackgroundContext bc (g, r);

  bc.drawImageAt (m_sourceImage [0], r.getX (), r.getY ());

  vf::LayerContext lc (bc, r);

  lc.setBlendMode (m_blendMode);
  lc.drawImageAt (m_sourceImage [1], r.getX (), r.getY ());
}

void CBlendModeResult::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
  m_blendMode = static_cast <vf::BlendMode> (m_comboBox->getSelectedId ());

  repaint ();
}

