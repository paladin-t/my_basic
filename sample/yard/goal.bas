REM Yet Another RPG Dungeon is a text based game.
REM It's aimed to be a comprehensive example and a tutorial of MY-BASIC.
REM For more information, see https://github.com/paladin-t/my_basic/

import "entity.bas"

class goal(entity)
	var take_handler = nil

	def to_string()
		return "Goal [" + name + "]"
	enddef

	def take(e)
		if take_handler <> nil then
			take_handler(me)
		endif
	enddef
endclass
