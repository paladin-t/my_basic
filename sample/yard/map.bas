REM Yet Another RPG Dungeon is a text based game.
REM It's aimed to be a comprehensive example and a tutorial of MY-BASIC.
REM For more information, see https://github.com/paladin-t/my_basic/

import "utils.bas"

class dirs
	var none_dir = 0
	var left_dir = 1
	var right_dir = 2
	var up_dir = 4
	var down_dir = 8
endclass

class map_node
	var pos = new(point)
	var valid_dirs = dirs.none_dir
	var entities = list()

	def to_string()
		s = pos.to_string()
		return "Map Node " + s
	enddef

	def set_valid_dirs(...)
		dir = 0
		while len(...)
			dir = bit_or(dir, ...)
		wend
		valid_dirs = dir
	enddef

	def add_entity(ent)
		if exists(entities, ent) then
			log.w("Already added " + ent.name)

			return false
		endif

		push(entities, ent)

		return true
	enddef

	def get_entities(y)
		ret = list()
		it = iterator(entities)
		while move_next(it)
			ent = get(it)
			if ent is y and ent.alive then
				push(ret, ent);
			endif
		wend

		return ret
	enddef
endclass

class map
	var width = 0
	var height = 0

	var nodes = dict()

	def to_string()
		return "Map"
	enddef

	def hash_pos(x, y)
		return x + y * width
	enddef

	def set_size(w, h)
		width = w
		height = h

		clear(nodes)
	enddef

	def set_node(x, y, n)
		h = hash_pos(x, y)
		n.pos.x = x
		n.pos.y = y
		nodes(h) = n

		return n
	enddef

	def get_node(x, y)
		h = hash_pos(x, y)

		return nodes(h)
	enddef
endclass
