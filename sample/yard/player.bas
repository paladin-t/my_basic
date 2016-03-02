' Yet Another RPG Dungeon is a text based game.
' It's aimed to be a comprehensive example and or a tutorial of MY-BASIC.
' Copyright (C) 2016 Wang Renxin. All rights reserved.
' For more information about MY-BASIC, see https://github.com/paladin-t/my_basic/

import "entity.bas"
import "utils.bas"

class player(entity)
	var pos = new(point)

	def tostring()
		return "Player [" + name + "]"
	enddef
endclass
