/*	
	This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020  Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "../gfx/sprite.h"

#include "../base/SDL_util.h"
#include "../base/db.h"
#include "../gfx/constants.h"
#include "range/v3/action/split.hpp"
#include "spdlog/spdlog.h"

#include <SDL_surface.h>

#include <range/v3/view.hpp>

Sprite::Sprite()
{
}

Sprite::Sprite( const Sprite& other )
{
	uID           = other.uID;
	sID           = other.sID;
	xOffset       = other.xOffset;
	yOffset       = other.yOffset;
	opacity       = other.opacity;
	randomNumbers = other.randomNumbers;
}

Sprite::~Sprite()
{
}

SpritePixmap::SpritePixmap( SDL_Surface* pixmap ) :
	Sprite(), m_pixmap(pixmap)
{
	m_type   = "pixmap";
}

SpritePixmap::SpritePixmap( SDL_Surface* pixmap, std::string offset ) :
	Sprite()
{
	m_type          = "pixmap";
	int xOffset     = 0;
	int yOffset     = 0;
	const auto osl  = ranges::actions::split( offset, " " );
	if ( osl.size() == 2 )
	{
		xOffset = std::stoi( osl[0] );
		yOffset = std::stoi( osl[1] );
	}

	auto* target = createPixmap( SpriteWidth, SpriteHeight );
	SDL_FillRect(target, nullptr, 0x00000000);

	copyPixmap(target, pixmap, xOffset, yOffset);

	m_pixmap = target;
}

SpritePixmap::SpritePixmap( const SpritePixmap& other ) :
	Sprite( other )
{
	m_type   = "pixmap";
	m_pixmap = other.m_pixmap;
	m_pixmap->refcount++;
}

SpritePixmap::~SpritePixmap()
{
	SDL_FreeSurface(m_pixmap);
}

SDL_Surface* SpritePixmap::pixmap( std::string season, unsigned char rotation, unsigned char animationStep )
{
	return m_pixmap;
}

void SpritePixmap::setPixmap( SDL_Surface* pm, std::string season, unsigned char rotation )
{
	m_pixmap = pm;
	m_pixmap->refcount++;
}

void SpritePixmap::applyEffect( std::string effect )
{
	// TODO: Why is this not part of the shader code?
	if ( effect == "FlipHorizontal" )
	{
		auto *old = m_pixmap;
		m_pixmap  = clonePixmap(old);
		SDL_FreeSurface(old);

		flipPixmap( m_pixmap, true, false );
	}
	else if ( effect == "Rot90" )
	{
		auto *old = m_pixmap;
		m_pixmap  = clonePixmap(old);
		SDL_FreeSurface(old);

		SDL_LockSurface(m_pixmap);
		// Expecting pixmap to be 32 bit
		for ( int y = 0; y < m_pixmap->h; ++y )
		{
			for ( int x = 0; x < m_pixmap->w; ++x )
			{

			}
		}
		SDL_UnlockSurface(m_pixmap);
	}
}

void SpritePixmap::applyTint( std::string tint, std::string materialSID )
{
	// TODO: Why is this not part of the shader code?
	if ( tint.empty() )
		return;

	SDL_Color color;

	if ( tint == "Material" )
	{
		tint = DB::select( "Color", "Materials", QString::fromStdString(materialSID) ).toString().toStdString();
	}
	color   = string2SDLColor( tint );
	opacity = color.a / 255.0f;

	tintPixmap( m_pixmap, color );
}

void SpritePixmap::combine( Sprite* other, std::string season, unsigned char rotation, unsigned char animationStep )
{
	if ( m_pixmap->w > 0 && m_pixmap->h > 0 )
	{
		copyPixmap( m_pixmap, other->pixmap( season, rotation, animationStep ), 0, 0 );
	}
}

SpriteSeasons::SpriteSeasons() :
	Sprite()
{
	m_type = "seasons";
}

SpriteSeasons::SpriteSeasons( const SpriteSeasons& other ) :
	Sprite( other )
{
	m_type    = "seasons";
	m_sprites = other.m_sprites;
}

SpriteSeasons::~SpriteSeasons()
{
	for ( auto s : m_sprites | ranges::views::values )
	{
		delete s;
	}
}

SDL_Surface* SpriteSeasons::pixmap( std::string season, unsigned char rotation, unsigned char animationStep )
{
	return m_sprites[season]->pixmap( season, rotation, animationStep );
}

void SpriteSeasons::setPixmap( SDL_Surface* pm, std::string season, unsigned char rotation )
{
	m_sprites[season]->setPixmap( pm, season, rotation );
}

void SpriteSeasons::applyEffect( std::string effect )
{
	for ( auto s : m_sprites | ranges::views::values )
	{
		s->applyEffect( effect );
	}
}

void SpriteSeasons::applyTint( std::string tint, std::string materialSID )
{
	for ( auto s : m_sprites | ranges::views::values )
	{
		s->applyTint( tint, materialSID );
	}
}

void SpriteSeasons::combine( Sprite* other, std::string season, unsigned char rotation, unsigned char animationStep )
{
	m_sprites[season]->combine( other, season, rotation, animationStep );
}

SpriteRotations::SpriteRotations() :
	Sprite()
{
	m_type = "rotations";
}

SpriteRotations::~SpriteRotations()
{
	for ( auto s : m_sprites )
	{
		delete s;
	}
}

SpriteRotations::SpriteRotations( const SpriteRotations& other ) :
	Sprite( other )
{
	m_type    = "rotations";
	m_sprites = other.m_sprites;
}

SDL_Surface* SpriteRotations::pixmap( std::string season, unsigned char rotation, unsigned char animationStep )
{
	return m_sprites[rotation]->pixmap( season, rotation, animationStep );
}

void SpriteRotations::setPixmap( SDL_Surface* pm, std::string season, unsigned char rotation )
{
	m_sprites[rotation]->setPixmap( pm, season, rotation );
}

void SpriteRotations::applyEffect( std::string effect )
{
	for ( auto s : m_sprites )
	{
		s->applyEffect( effect );
	}
}

void SpriteRotations::applyTint( std::string tint, std::string materialSID )
{
	for ( auto s : m_sprites )
	{
		s->applyTint( tint, materialSID );
	}
}

void SpriteRotations::combine( Sprite* other, std::string season, unsigned char rotation, unsigned char animationStep )
{
	m_sprites[rotation]->combine( other, season, rotation, animationStep );
}

SpriteFrames::SpriteFrames() :
	Sprite()
{
	m_type = "frames";
}

SpriteFrames::SpriteFrames( const SpriteFrames& other ) :
	Sprite( other )
{
	m_type    = "frames";
	m_sprites = other.m_sprites;
}

SpriteFrames::~SpriteFrames()
{
	for ( auto s : m_sprites )
	{
		delete s;
	}
}

SDL_Surface* SpriteFrames::pixmap( std::string season, unsigned char rotation, unsigned char animationStep )
{
	return m_sprites[animationStep % m_sprites.size()]->pixmap( season, rotation, animationStep );
}

void SpriteFrames::setPixmap( SDL_Surface* pm, std::string season, unsigned char rotation )
{
	m_sprites[0]->setPixmap( pm, season, rotation );
}

void SpriteFrames::applyEffect( std::string effect )
{
	for ( auto s : m_sprites )
	{
		s->applyEffect( effect );
	}
}

void SpriteFrames::applyTint( std::string tint, std::string materialSID )
{
	for ( auto s : m_sprites )
	{
		s->applyTint( tint, materialSID );
	}
}

void SpriteFrames::combine( Sprite* other, std::string season, unsigned char rotation, unsigned char animationStep )
{
	if ( m_sprites.size() > animationStep )
	{
		m_sprites[animationStep]->combine( other, season, rotation, animationStep );
	}
}
