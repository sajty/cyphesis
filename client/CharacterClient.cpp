// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "CharacterClient.h"

#include "ClientConnection.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation/RootOperation.h>

static const bool debug_flag = false;

CharacterClient::CharacterClient(const std::string & id,
                                 const std::string & name,
                                 ClientConnection & c) :
                                 BaseMind(id, name), m_connection(c)
{
}

void CharacterClient::sightImaginaryOperation(const Operation &,
                                              Operation &, OpVector &)
{
}

void CharacterClient::soundTalkOperation(const Operation & ,
                                         Operation &, OpVector &)
{
}

void CharacterClient::send(Operation & op)
{
    op.setFrom(getId());
    m_connection.send(op);
}

int CharacterClient::sendAndWaitReply(Operation & op, OpVector & res)
{
    send(op);
    long no = op.getSerialno();
    while (true) {
        if (m_connection.pending()) {
            Operation * input = CharacterClient::m_connection.pop();
            if (input != NULL) {
                // What the hell is this!
                OpVector result;
                operation(*input, result);
                OpVector::const_iterator I = result.begin();
                OpVector::const_iterator Iend = result.end();
                for (; I != Iend; ++I) {
                    send(*(*I));
                }
    
                if (input->getRefno() == no) {
                    res.push_back(input);
                    return 0;
                }
                delete input;
            }
        } else if (m_connection.wait() != 0) {
            return -1;
        }
    }
}
