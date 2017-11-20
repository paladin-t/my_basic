REM Yet Another RPG Dungeon is a text based game.
REM It's aimed to be a comprehensive example and a tutorial of MY-BASIC.
REM For more information, see https://github.com/paladin-t/my_basic/

import "entity.bas"

class npc(entity)
	var talk_handler = nil

	def to_string()
		return "NPC [" + name + "]"
	enddef

	def talk(e)
		if talk_handler <> nil then
			talk_handler(me)
		endif
	enddef
endclass
