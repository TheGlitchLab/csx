////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#include "io/ioguild.hpp"

#include "database/database.hpp"
#include "creatures/players/grouping/guild.hpp"

std::shared_ptr<Guild> IOGuild::loadGuild(uint32_t guildId) {
	Database &db = Database::getInstance();
	std::ostringstream query;
	query << "SELECT `name`, `balance` FROM `guilds` WHERE `id` = " << guildId;
	if (DBResult_ptr result = db.storeQuery(query.str())) {
		const auto guild = std::make_shared<Guild>(guildId, result->getString("name"));
		guild->setBankBalance(result->getNumber<uint64_t>("balance"));
		query.str(std::string());
		query << "SELECT `id`, `name`, `level` FROM `guild_ranks` WHERE `guild_id` = " << guildId;

		if ((result = db.storeQuery(query.str()))) {
			do {
				guild->addRank(result->getNumber<uint32_t>("id"), result->getString("name"), result->getNumber<uint16_t>("level"));
			} while (result->next());
		}
		return guild;
	}
	return nullptr;
}

void IOGuild::saveGuild(const std::shared_ptr<Guild> &guild) {
	if (!guild) {
		return;
	}
	Database &db = Database::getInstance();
	std::ostringstream updateQuery;
	updateQuery << "UPDATE `guilds` SET ";
	updateQuery << "`balance` = " << guild->getBankBalance();
	updateQuery << " WHERE `id` = " << guild->getId();
	db.executeQuery(updateQuery.str());
}

uint32_t IOGuild::getGuildIdByName(const std::string &name) {
	Database &db = Database::getInstance();

	std::ostringstream query;
	query << "SELECT `id` FROM `guilds` WHERE `name` = " << db.escapeString(name);

	DBResult_ptr result = db.storeQuery(query.str());
	if (!result) {
		return 0;
	}
	return result->getNumber<uint32_t>("id");
}

void IOGuild::getWarList(uint32_t guildId, GuildWarVector &guildWarVector) {
	std::ostringstream query;
	query << "SELECT `guild1`, `guild2` FROM `guild_wars` WHERE (`guild1` = " << guildId << " OR `guild2` = " << guildId << ") AND `status` = 1";

	DBResult_ptr result = Database::getInstance().storeQuery(query.str());
	if (!result) {
		return;
	}

	do {
		auto guild1 = result->getNumber<uint32_t>("guild1");
		if (guildId != guild1) {
			guildWarVector.push_back(guild1);
		} else {
			guildWarVector.push_back(result->getNumber<uint32_t>("guild2"));
		}
	} while (result->next());
}
