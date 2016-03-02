' Yet Another RPG Dungeon is a text based game.
' It's aimed to be a comprehensive example and or a tutorial of MY-BASIC.
' Copyright (C) 2016 Wang Renxin. All rights reserved.
' For more information about MY-BASIC, see https://github.com/paladin-t/my_basic/

import "entity.bas"

class goal(entity)
	var take_handler = nil

	def tostring()
		return "Goal [" + name + "]"
	enddef

	def take(e)
		if take_handler <> nil then
			take_handler(me)
		endif
	enddef
endclass
