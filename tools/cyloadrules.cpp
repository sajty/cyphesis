// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2004 Alistair Riddoch

#include "common/Database.h"
#include "common/globals.h"

#include <Atlas/Message/DecoderBase.h>
// #include <Atlas/Message/Element.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>
#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

class RuleBase {
  protected:
    RuleBase() : m_connection(*Database::instance()) { }

    Database & m_connection;
    static RuleBase * m_instance;
    std::string m_rulesetName;
  public:
    ~RuleBase() {
        m_connection.shutdownConnection();
    }

    static RuleBase * instance() {
        if (m_instance == NULL) {
            m_instance = new RuleBase();
            if (m_instance->m_connection.initConnection(true) != 0) {
                delete m_instance;
                m_instance = 0;
            } else if (!m_instance->m_connection.initRule(true)) {
                delete m_instance;
                m_instance = 0;
            }
        }
        return m_instance;
    }

    void storeInRules(const MapType & o, const std::string & key) {
        if (m_connection.hasKey(m_connection.rule(), key)) {
            return;
        }
        m_connection.putObject(m_connection.rule(), key, o, StringVector(1, m_rulesetName));
        if (!m_connection.clearPendingQuery()) {
            std::cerr << "Failed" << std::endl << std::flush;
        }
    }
    bool clearRules() {
        return (m_connection.clearTable(m_connection.rule()) &&
                m_connection.clearPendingQuery());
    }
    void setRuleset(const std::string & n) {
        m_rulesetName = n;
    }
};

RuleBase * RuleBase::m_instance = NULL;

class FileDecoder : public Atlas::Message::DecoderBase {
    std::fstream m_file;
    RuleBase & m_db;
    Atlas::Codecs::XML m_codec;
    int m_count;

    virtual void messageArrived(const MapType & omap) {
        MapType::const_iterator I = omap.find("id");
        if (I == omap.end()) {
            std::cerr << "Found rule with no id" << std::endl << std::flush;
            return;
        }
        if (!I->second.isString()) {
            std::cerr << "Found rule with non string id" << std::endl << std::flush;
            return;
        }
        m_count++;
        m_db.storeInRules(omap, I->second.asString());
    }
  public:
    FileDecoder(const std::string & filename, RuleBase & db) :
                m_file(filename.c_str(), std::ios::in), m_db(db),
                m_codec(m_file, *this), m_count(0)
    {
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    void report() {
        std::cout << m_count << " classes stored in rule database."
                  << std::endl << std::flush;
    }

    bool isOpen() {
        return m_file.is_open();
    }
};

static void usage(char * prgname)
{
    std::cerr << "usage: " << prgname << " [<rulesetname> <atlas-xml-file>]" << std::endl << std::flush;
}

int main(int argc, char ** argv)
{
    int optind;

    if ((optind = loadConfig(argc, argv)) < 0) {
        // Fatal error loading config file
        return 1;
    }

    RuleBase * db = RuleBase::instance();

    if (db == 0) {
        std::cerr << argv[0] << ": Could not make database connection."
                  << std::endl << std::flush;
        return 1;
    }

    if (optind == (argc - 2)) {
        FileDecoder f(argv[optind + 1], *db);
        if (!f.isOpen()) {
            std::cerr << "ERROR: Unable to open file " << argv[optind + 1]
                      << std::endl << std::flush;
            return 1;
        }
        db->setRuleset(argv[optind]);
        f.read();
        f.report();
    } else if (optind == argc) {
        db->clearRules();
        std::vector<std::string>::const_iterator I = rulesets.begin();
        std::vector<std::string>::const_iterator Iend = rulesets.end();
        for (; I != Iend; ++I) {
            std::cout << "Reading rules from " << *I << std::endl << std::flush;
            std::string filename = etc_directory + "/cyphesis/" + *I + ".xml";
            FileDecoder f(filename, *db);
            if (!f.isOpen()) {
                std::cerr << "ERROR: Unable to open file " << filename
                          << std::endl << std::flush;
                return 1;
            }
            db->setRuleset(*I);
            f.read();
            f.report();
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    delete db;
}
