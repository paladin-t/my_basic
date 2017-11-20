REM Yet Another RPG Dungeon is a text based game.
REM It's aimed to be a comprehensive example and a tutorial of MY-BASIC.
REM For more information, see https://github.com/paladin-t/my_basic/

import "entity.bas"
import "utils.bas"

class player(entity)
	var pos = new(point)

	def to_string()
		return "Player [" + name + "]"
	enddef
endclass
