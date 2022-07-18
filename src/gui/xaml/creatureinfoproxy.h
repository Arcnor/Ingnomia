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
#pragma once

#include "../aggregatorcreatureinfo.h"
#include "creatureinfomodel.h"

#include <QObject>

#include <sigslot/signal.hpp>

class CreatureInfoProxy : public QObject
{
	Q_OBJECT

public:
	CreatureInfoProxy( QObject* parent = nullptr );
	~CreatureInfoProxy();

	void setParent( IngnomiaGUI::CreatureInfoModel* parent );
	void requestProfessionList();
	void setProfession( unsigned int gnomeID, const std::string& profession );

	void requestEmptySlotImages();


private:
	IngnomiaGUI::CreatureInfoModel* m_parent = nullptr;

private slots:
	void onUpdateInfo( const GuiCreatureInfo& info );
	void onProfessionList( const std::vector<std::string>& profs );
	void onEmptyPics( const absl::btree_map< QString, std::vector<unsigned char> >& emptyPics );
public: // signals:
	sigslot::signal<unsigned int /*creatureID*/> signalRequestCreatureUpdate;
	sigslot::signal<> signalRequestProfessionList;
	sigslot::signal<unsigned int /*gnomeID*/, const std::string& /*profession*/> signalSetProfession;
	sigslot::signal<> signalRequestEmptySlotImages;
};
