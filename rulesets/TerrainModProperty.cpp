// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: TerrainModProperty.cpp,v 1.2 2008-08-17 18:12:10 alriddoch Exp $

#include "TerrainModProperty.h"

#include "World.h"

#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include "TerrainProperty.h"

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

typedef Mercator::Terrain::Pointstore Pointstore;
typedef Mercator::Terrain::Pointcolumn Pointcolumn;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty() : PropertyBase(0), m_owner(0), m_modptr(0)
{
}

void TerrainModProperty::setup(Entity* owner)
{
    log(INFO, "setting up terrain mods");
    m_owner = owner;
    Element ent;
    owner->getAttr("terrainmod", ent);
    m_terrainmods = ent.asMap();

//     SetTerrainModifiers(m_owner, ent, m_owner->m_location.pos());
    set(ent);
}

bool TerrainModProperty::get(Element & ent) const
{
    MapType & mod = (ent = MapType()).Map();
    mod = m_terrainmods;
    return true;
}

Mercator::TerrainMod * TerrainModProperty::getModifier(Entity* owner)
{
//     return parseModData(owner, m_terrainmods, owner->m_location.pos());
    return m_modptr;
}

Mercator::TerrainMod * TerrainModProperty::getModifier()
{
//     return parseModData(m_owner, m_terrainmods, m_owner->m_location.pos());
    return m_modptr;
}

void TerrainModProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_terrainmods = mod;
    }

    if (m_owner != NULL) {
//         SetTerrainModifiers(m_owner, ent, m_owner->m_location.pos());
        // Find the terrain
        const EntityDict & ents = BaseWorld::instance().getEntities();
        EntityDict::const_iterator eI = ents.begin();
        PropertyBase * terr; // terrain property?
        TerrainProperty * r_terr; // real terrain property
            // Search for an entity with the terrain property
        for (; eI != ents.end(); eI++)
        {
            terr = eI->second->getProperty("terrain");
            if (terr != NULL) {
                break;
            }
        }

        if (terr != NULL) {
            r_terr = dynamic_cast<TerrainProperty*>(terr);

                // If we're updating an existing mod, remove it from the terrain first
            if (m_modptr != NULL) {
                r_terr->removeMod(m_modptr);
            }

                // Parse the Atlas data for our mod
            Mercator::TerrainMod *newMod = parseModData(ent);
                // Apply the new mod to the terrain; retain the returned pointer
            m_modptr = r_terr->setMod(newMod);
        }
    }
}

void TerrainModProperty::setPos(Point3D newPos)
{
    if (m_owner != NULL) {
        // Find the terrain
        const EntityDict & ents = BaseWorld::instance().getEntities();
        EntityDict::const_iterator eI = ents.begin();
        PropertyBase * terr; // terrain property?
        TerrainProperty * r_terr; // real terrain property
            // Search for an entity with the terrain property
        for (; eI != ents.end(); eI++)
        {
            terr = eI->second->getProperty("terrain");
            if (terr != NULL) {
                break;
            }
        }

        if (terr != NULL) {
            r_terr = dynamic_cast<TerrainProperty*>(terr);

                // If we're updating an existing mod, remove it from the terrain first
            if (m_modptr != NULL) {
                r_terr->removeMod(m_modptr);
            }

                // Parse the Atlas data for our mod, using the new position
            Mercator::TerrainMod *newMod = parseModData(m_terrainmods, newPos);
                // Apply the new mod to the terrain; retain the returned pointer
            m_modptr = r_terr->setMod(newMod);
        }
    }
}

void TerrainModProperty::add(const std::string & s, MapType & ent) const
{
    get(ent[s]);
}

void TerrainModProperty::move(Entity* owner, Point3D newPos)
{
        // Get terrain
    const EntityDict & ents = BaseWorld::instance().getEntities();
    EntityDict::const_iterator eI = ents.begin();
    PropertyBase * prop;
    TerrainProperty * terrain;

    for (; eI != ents.end(); eI++)
    {
        prop = eI->second->getProperty("terrain");
        if (prop != NULL) {
            terrain = dynamic_cast<TerrainProperty*>(prop);
            break; // Found the terrain!
        }
    }
    
        // Clear the mod from the old position
    terrain->removeMod(m_modptr);

        // Apply the mod at the new position
    setPos(newPos);
}

Mercator::TerrainMod * TerrainModProperty::parseModData(const Element & modifier)
{
    if (!modifier.isMap()) {
        log(ERROR, "Invalid terrain mod data");
        return NULL;
    }

    const Atlas::Message::MapType & modMap = modifier.asMap();
    m_terrainmods = modMap;
    std::string modType;
    std::string shapeType;
    int shapeDim;
//     WFMath::Point<3> pos;
    Atlas::Message::MapType shapeMap;

    Atlas::Message::MapType::const_iterator mod_I;

    // Get modifier type
    mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
    const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            modType = modTypeElem.asString();
        }
    }

    // Get modifier position
    Point3D modPos = m_owner->m_location.pos();
    WFMath::Point<3> pos = WFMath::Point<3>(modPos.x(), modPos.y(), modPos.z());


    // Get modifier's shape
    mod_I = modMap.find("shape");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& shapeElem(mod_I->second);
        if (shapeElem.isMap()) {
            shapeMap = shapeElem.asMap();
            Atlas::Message::MapType::const_iterator shape_I;
                // Get shape's type
            shape_I = shapeMap.find("type");
            if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeTypeElem(shape_I->second);
                if (shapeTypeElem.isString()) {
                shapeType = shapeTypeElem.asString();
                }
            }

            // Get shape's dimension
            shape_I = shapeMap.find("dim");
            if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeDimElem(shape_I->second);
                if (shapeDimElem.isInt()) {
                    shapeDim = (int)shapeDimElem.asNum();
                }
            }
        } // end shape data

        // Check for additional modifier parameters
        if (modType == "slopemod") {
            float dx, dy, level;
            // Get slopes
            mod_I = modMap.find("slopes");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modSlopeElem = mod_I->second;
                if (modSlopeElem.isList()) {
                    const Atlas::Message::ListType & slopes = modSlopeElem.asList();
                    dx = (int)slopes[0].asNum();
                    dy = (int)slopes[1].asNum();
                }
            }

            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                shapeRadius = shapeRadiusElem.asNum();
                }

                // Make disc
                WFMath::Point<2> pos_2d(pos.x(),pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

//                  log(INFO, "Successfully parsed a slopemod");

                // Make modifier

                // Apply Modifier

            }

        } else if (modType == "levelmod") {
            float level;
            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make disc
                WFMath::Point<2> pos_2d(pos.x(),pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius); ///FIXME: assumes 2d ball...

//                  log(INFO, "Successfully parsed a levelmod");

                // Make Modifier
                Mercator::LevelTerrainMod<WFMath::Ball<2> > *NewMod;
                NewMod = new Mercator::LevelTerrainMod<WFMath::Ball<2> >(level, modShape);

                return NewMod;

            } else if (shapeType == "rotbox") {
                WFMath::Point<2> shapePoint;
                WFMath::Vector<2> shapeVector;
                // Get rotbox's position
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("point");
                if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapePointElem(shape_I->second);
                    if (shapePointElem.isList()) {
                        const Atlas::Message::ListType & pointList = shapePointElem.asList();
                        shapePoint = WFMath::Point<2>((int)pointList[0].asNum(), (int)pointList[1].asNum());
                    }
                }
                // Get rotbox's vector
                shape_I = shapeMap.find("vector");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeVectorElem(shape_I->second);
                    if (shapeVectorElem.isList()) {
                        const Atlas::Message::ListType & vectorList = shapeVectorElem.asList(); 
                        shapeVector = WFMath::Vector<2>((int)vectorList[0].asNum(), (int)vectorList[1].asNum());
                    }
                }
                
//                 log(INFO,"Successfully parsed a levelmod");

                // Make rotbox
                    ///FIXME: needs to use shapeDim instead of 2
                WFMath::RotBox<2> modShape = WFMath::RotBox<2>(shapePoint, shapeVector, WFMath::RotMatrix<2>()); 

                // Make modifier
                Mercator::LevelTerrainMod<WFMath::RotBox<2> > *NewMod;
                NewMod = new Mercator::LevelTerrainMod<WFMath::RotBox<2> >(level, modShape);

                return NewMod;
            }

        } else if (modType == "adjustmod") {
            float level;
            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make sphere
                WFMath::Point<2> pos_2d(pos.x(), pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

                // Make modifier

                // Apply Modifier

            }
//             log(INFO,"Successfully parsed an adjustmod");

        } else if (modType == "cratermod") {

            // Get other shape parameters
            if (shapeType == "ball" ) {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make sphere
                WFMath::Ball<3> modShape = WFMath::Ball<3>(pos, shapeRadius); ///FIXME: assumes 3d ball...

//                 log(INFO,"Successfully parsed a cratermod");
                // Make modifier
                Mercator::CraterTerrainMod *NewMod;
                NewMod = new Mercator::CraterTerrainMod(modShape);

                return NewMod;
            }
        }
    }
}

Mercator::TerrainMod * TerrainModProperty::parseModData(const Element & modifier, Point3D newPos)
{
    if (!modifier.isMap()) {
        log(ERROR, "Invalid terrain mod data");
        return NULL;
    }

    const Atlas::Message::MapType & modMap = modifier.asMap();
    m_terrainmods = modMap;
    std::string modType;
    std::string shapeType;
    int shapeDim;
//     WFMath::Point<3> pos;
    Atlas::Message::MapType shapeMap;

    Atlas::Message::MapType::const_iterator mod_I;

    // Get modifier type
    mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
    const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            modType = modTypeElem.asString();
        }
    }

    // Get modifier position
    Point3D modPos = m_owner->m_location.pos();
    WFMath::Point<3> pos = WFMath::Point<3>(newPos.x(), newPos.y(), newPos.z());


    // Get modifier's shape
    mod_I = modMap.find("shape");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& shapeElem(mod_I->second);
        if (shapeElem.isMap()) {
            shapeMap = shapeElem.asMap();
            Atlas::Message::MapType::const_iterator shape_I;
                // Get shape's type
            shape_I = shapeMap.find("type");
            if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeTypeElem(shape_I->second);
                if (shapeTypeElem.isString()) {
                shapeType = shapeTypeElem.asString();
                }
            }

            // Get shape's dimension
            shape_I = shapeMap.find("dim");
            if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeDimElem(shape_I->second);
                if (shapeDimElem.isInt()) {
                    shapeDim = (int)shapeDimElem.asNum();
                }
            }
        } // end shape data

        // Check for additional modifier parameters
        if (modType == "slopemod") {
            float dx, dy, level;
            // Get slopes
            mod_I = modMap.find("slopes");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modSlopeElem = mod_I->second;
                if (modSlopeElem.isList()) {
                    const Atlas::Message::ListType & slopes = modSlopeElem.asList();
                    dx = (int)slopes[0].asNum();
                    dy = (int)slopes[1].asNum();
                }
            }

            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                shapeRadius = shapeRadiusElem.asNum();
                }

                // Make disc
                WFMath::Point<2> pos_2d(pos.x(),pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

//                  log(INFO, "Successfully parsed a slopemod");

                // Make modifier

                // Apply Modifier

            }

        } else if (modType == "levelmod") {
            float level;
            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make disc
                WFMath::Point<2> pos_2d(pos.x(),pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius); ///FIXME: assumes 2d ball...

//                  log(INFO, "Successfully parsed a levelmod");

                // Make Modifier
                Mercator::LevelTerrainMod<WFMath::Ball<2> > *NewMod;
                NewMod = new Mercator::LevelTerrainMod<WFMath::Ball<2> >(level, modShape);

                return NewMod;

            } else if (shapeType == "rotbox") {
                WFMath::Point<2> shapePoint;
                WFMath::Vector<2> shapeVector;
                // Get rotbox's position
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("point");
                if (shape_I != shapeMap.end()) {
                const Atlas::Message::Element& shapePointElem(shape_I->second);
                    if (shapePointElem.isList()) {
                        const Atlas::Message::ListType & pointList = shapePointElem.asList();
                        shapePoint = WFMath::Point<2>((int)pointList[0].asNum(), (int)pointList[1].asNum());
                    }
                }
                // Get rotbox's vector
                shape_I = shapeMap.find("vector");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeVectorElem(shape_I->second);
                    if (shapeVectorElem.isList()) {
                        const Atlas::Message::ListType & vectorList = shapeVectorElem.asList(); 
                        shapeVector = WFMath::Vector<2>((int)vectorList[0].asNum(), (int)vectorList[1].asNum());
                    }
                }
                
//                 log(INFO,"Successfully parsed a levelmod");

                // Make rotbox
                    ///FIXME: needs to use shapeDim instead of 2
                WFMath::RotBox<2> modShape = WFMath::RotBox<2>(shapePoint, shapeVector, WFMath::RotMatrix<2>()); 

                // Make modifier
                Mercator::LevelTerrainMod<WFMath::RotBox<2> > *NewMod;
                NewMod = new Mercator::LevelTerrainMod<WFMath::RotBox<2> >(level, modShape);

                return NewMod;
            }

        } else if (modType == "adjustmod") {
            float level;
            // Get level
            mod_I = modMap.find("height");
            if (mod_I != modMap.end()) {
                const Atlas::Message::Element& modHeightElem = mod_I->second;
                level = modHeightElem.asNum();
            }

            if (shapeType == "ball") {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make sphere
                WFMath::Point<2> pos_2d(pos.x(), pos.y());
                WFMath::Ball<2> modShape = WFMath::Ball<2>(pos_2d, shapeRadius);

                // Make modifier

                // Apply Modifier

            }
//             log(INFO,"Successfully parsed an adjustmod");

        } else if (modType == "cratermod") {

            // Get other shape parameters
            if (shapeType == "ball" ) {
                float shapeRadius;
                // Get sphere's radius
                Atlas::Message::MapType::const_iterator shape_I = shapeMap.find("radius");
                if (shape_I != shapeMap.end()) {
                    const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
                    shapeRadius = shapeRadiusElem.asNum();
                }

                // Make sphere
                WFMath::Ball<3> modShape = WFMath::Ball<3>(pos, shapeRadius); ///FIXME: assumes 3d ball...

//                 log(INFO,"Successfully parsed a cratermod");
                // Make modifier
                Mercator::CraterTerrainMod *NewMod;
                NewMod = new Mercator::CraterTerrainMod(modShape);

                return NewMod;
            }
        }
    }
}