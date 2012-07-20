/*============================================================================*/
/*
  Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
  See the file GNU_GPL_v2.txt for full licensing terms.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*============================================================================*/

LayerContext::LayerContext (BackgroundContext& destinationContext,
                            Rectangle <int> const& drawBounds)
  : ContextImageBase (
      destinationContext.getImageBounds ().getIntersection (drawBounds),
      Image::ARGB)
  , Graphics (getImage ())
  , m_destinationContext (destinationContext)
{
  getImage ().clear (getImage ().getBounds (), Colour::fromRGBA (0, 0, 0, 0));
  
  setOrigin (-getImageBounds ().getX (), -getImageBounds ().getY ());
} 

//------------------------------------------------------------------------------

LayerContext::~LayerContext ()
{
  Image workImage (
    Image::RGB,
    getImageBounds ().getWidth (),
    getImageBounds ().getHeight (),
    false);

  // Copy the background layer into the work image.
  copyImage (workImage,
             Point <int> (0, 0),
             m_destinationContext.getImage (),
             getImageBounds (),
             normal,
             1);

  // Draw effects from the bottom up.

  applyDropShadow (workImage);

  applyFill (workImage);

  applyInnerShadow (workImage);

  // Copy the work image onto the background layer
  // using normal mode and the general opacity.
  copyImage (m_destinationContext.getImage (),
             getImageBounds ().getTopLeft (),
             workImage,
             workImage.getBounds (),
             normal,
             m_options.general.opacity);
}

//------------------------------------------------------------------------------

LayerContext::Options& LayerContext::getOptions ()
{
  return m_options;
}

//------------------------------------------------------------------------------

void LayerContext::applyDropShadow (Image& workImage)
{
  Options::DropShadow& dropShadow = m_options.dropShadow;

  if (!dropShadow.active)
    return;

  int const dx = static_cast <int> (
    - dropShadow.distance * std::cos (dropShadow.angle) + 0.5) - dropShadow.size;
  
  int const dy = static_cast <int> (
    dropShadow.distance * std::sin (dropShadow.angle) + 0.5) - dropShadow.size;

  // Get the layer mask as an individual channel.
  Image mask = ChannelImageType::fromImage (getImage (), 3);
  
  RadialImageConvolutionKernel k (dropShadow.size + 1);
  k.createGaussianBlur ();

  // Compute the shadow mask.
  Image shadow = k.createConvolvedImageFull (mask);

  // Optionally subtract layer mask from shadow mask.
  if (dropShadow.knockout)
    copyImage (
      shadow,
      Point <int> (-dx, -dy),
      mask,
      mask.getBounds (),
      subtract,
      1);

  // Fill the shadow mask.
  fillImage (workImage,
             getImageBounds ().getTopLeft (),
             shadow,
             shadow.getBounds (),
             dropShadow.mode,
             dropShadow.opacity,
             dropShadow.colour);
}

//------------------------------------------------------------------------------

static void InvertImage (Image image)
{
  switch (image.getFormat ())
  {
  case Image::SingleChannel:
    {
      Image::BitmapData bits (image, Image::BitmapData::readWrite);

      uint8* dest = bits.getLinePointer (0);
      int const rowBytes = bits.lineStride - bits.width * bits.pixelStride;

      for (int y = bits.height; y--;)
      {
        for (int x = bits.width; x--;)
        {
          *dest = 255 - *dest;
          dest += bits.pixelStride;
        }

        dest += rowBytes;
      }
    }
    break;

  default:
    jassertfalse;
    break;
  }
}

void LayerContext::applyInnerShadow (Image& workImage)
{
  Options::InnerShadow& innerShadow = m_options.innerShadow;

  if (!innerShadow.active)
    return;

  int const dx = static_cast <int> (
    - innerShadow.distance * std::cos (innerShadow.angle) + 0.5) - innerShadow.size;
  
  int const dy = static_cast <int> (
    innerShadow.distance * std::sin (innerShadow.angle) + 0.5) - innerShadow.size;

  Image mask = ChannelImageType::fromImage (getImage (), 3);
  
  RadialImageConvolutionKernel k (innerShadow.size + 1);
  k.createGaussianBlur ();

  Image shadow = k.createConvolvedImage (mask);

  InvertImage (shadow);

  // clip inverse shadow mask to interior of layer
  copyImage (shadow,
             Point <int> (0, 0),
             mask,
             mask.getBounds (),
             darken,
             1);
  // DO THIS CLIP FIRST so we have fewer pixels to process?
  //shadow = shadow.getClippedImage (mask.getBounds () + Point <int> (dx, dy));

  // Fill the shadow mask.
  fillImage (workImage,
             getImageBounds ().getTopLeft (),
             shadow,
             shadow.getBounds (),
             innerShadow.mode,
             innerShadow.opacity,
             innerShadow.colour);
}

//------------------------------------------------------------------------------

void LayerContext::applyFill (Image& workImage)
{
  copyImage (workImage,
             getImageBounds ().getTopLeft (),
             getImage (),
             getImageBounds (),
             m_options.fill.mode,
             m_options.fill.opacity);
}

//------------------------------------------------------------------------------
/*

Experimental Evidence:

- Inner Shadow draws on top of Drop Shadow
- Fill draws on top of Drop Shadow


*/
