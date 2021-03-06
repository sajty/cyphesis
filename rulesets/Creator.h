// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id$

#ifndef RULESETS_CREATOR_H
#define RULESETS_CREATOR_H

#include "Character.h"

typedef Character Creator_parent;

/// \brief This is a class for an in-game entity used by administrators
/// and world builders to manipulate the world
/// \ingroup EntityClasses
class Creator : public Creator_parent {
  public:
    explicit Creator(const std::string & id, long intId);

    virtual void operation(const Operation & op, OpVector &);
    virtual void externalOperation(const Operation & op);

    virtual void mindLookOperation(const Operation & op, OpVector &);
};

#endif // RULESETS_CREATOR_H
